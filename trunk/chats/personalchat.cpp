#include "personalchat.h"
#include "ui_personalchat.h"
#include <QWebFrame>
#include <QWebElement>
#include "chats.h"
#include "../vk/vkengine.h"
#include <QWebView>
#include "chattextedit.h"
#include <QDebug>
#include <QTime>
#include "../vk/vkmessage.h"
#include "../roster/qhttpimage.h"
#include <QDomNode>
#include <QDomNodeList>
#include <QDomElement>
#include <QTabWidget>
#include <QTimer>
#include "../vk/vkrequest.h"
#include <QMovie>
#include <QStandardItemModel>
#include <QStandardItem>
#include "messagestyler.h"
#ifdef QT_PHONON_LIB
#include "mediaplayer.h"
#include "vkmediafactory.h"
#endif
#include <QDeclarativeView>
#include "declarative/albumsmodel.h"
#include <QDeclarativeContext>
#include "videolistmodel.h"
#include <QDesktopServices>
#include "emoticons/chatemoticonmenu.h"
#include "../vimkamain.h"
#include "../settingsmanager.h"
#include "declarative/photoviewer.h"
#include <QDeclarativePropertyMap>
#include "../SlidingStackedWidget.h"

PersonalChat::PersonalChat( QString _fromId, Chats *chatsWindow,
                            VKEngine *engine,
                            #ifdef QT_PHONON_LIB
                            MediaPlayer *pagePlayer,
                            #endif
                            AlbumsModel *albumsModel) :
QWidget(0),
ui(new Ui::PersonalChat)
{
    ui->setupUi(this);
    #ifdef QT_PHONON_LIB
    this->pagePlayer = pagePlayer;
    #endif
    this->albumsModel = albumsModel;

    setObjectName(_fromId);

    sMn = engine->m_rosterWindow->settingsMngr;

    ui->tbEmoticons->setIcon(sMn->emoticon);
    ui->lblFromActivity->hide();

    connect(albumsModel, SIGNAL(photoIsSelected(QString, QString, QString, QString, QString, QString)),
            this, SLOT(photoIsSelected (QString, QString, QString, QString, QString, QString)));

    QMovie *movie = new QMovie(":/images/upload.gif");
    ui->lblUploadInfo->setMovie(movie);
    ui->lblUploadAudioInfo->setMovie(movie);
    ui->lblUploadVideoInfo->setMovie(movie);
    movie->start();
    ui->lblUploadInfo->hide();
    ui->lblUploadAudioInfo->hide();
    ui->lblUploadVideoInfo->hide();

    ui->stackedWidget->setCurrentWidget(ui->pageChat);

    isProfilesLoaded = false;

    incMessageMigalko = new QTimer();
    incMessageMigalko->setInterval(500);
    connect(incMessageMigalko, SIGNAL(timeout()),
            this,SLOT(onIncTimer()));
    onlineMess = false;

    fromId = _fromId;
    m_Chats = chatsWindow;
    m_vkEngine = engine;

    webView = new QWebView(this);
    webView->pageAction(QWebPage::Copy)->setShortcut(QKeySequence::Copy);

    //webView->page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
    viewScroller = new QTimer();
    viewScroller->setSingleShot(true);
    viewScroller->setInterval(200);
    connect(viewScroller, SIGNAL(timeout()), this, SLOT(scrollToBottom()));

    //act->setShortcutContext();
#ifdef QT_PHONON_LIB
    VKMediaFactory *factory = new VKMediaFactory(webView, pagePlayer);
    webView->page()->setPluginFactory(factory);
#endif

    ui->webkitWgt->layout()->addWidget(webView);
    webView->setHtml("");
    webView->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);

    chatEdit = new ChatTextEdit(m_vkEngine->m_rosterWindow, ui->chatTEWgt);
    ui->chatTEWgt->layout()->addWidget(chatEdit);

    bool check = connect(chatEdit, SIGNAL(returnPressed()),
                         this, SLOT(on_sendButton_clicked()));
    Q_ASSERT(check);
    check = connect(chatEdit, SIGNAL(focusInChatEdit()),
                    this, SLOT(stopMigalko()));
    Q_ASSERT(check);

    //Профили. Свои и собеседника
    connect(m_vkEngine, SIGNAL(requestFinished(VKRequest*)),
            this, SLOT(slotProfilesLoaded(VKRequest *)));

    //настройка таймера пометки прочитанных
    markFromIdMessagesAsReadTimer = new QTimer();
    markFromIdMessagesAsReadTimer->setSingleShot(true);
    markFromIdMessagesAsReadTimer->setInterval(3000);
    connect(markFromIdMessagesAsReadTimer,SIGNAL(timeout()),
            this,SLOT(markFromIdMessagesAsReadFinish()));
    connect(m_vkEngine, SIGNAL(requestFinished(VKRequest *)),
            this, SLOT(markResponsReceived(VKRequest *)));
    connect(m_vkEngine, SIGNAL(requestFinished(VKRequest *)),
            this, SLOT(sendMessageReceived(VKRequest *)));

    //история
    connect(m_vkEngine, SIGNAL(requestFinished(VKRequest *)),
            this, SLOT(getHistoryFinished(VKRequest *)));

    //аудио
    audioItems = new QStandardItemModel(0, 4, this);
    ui->lvAudio->setModel(audioItems);
    connect(m_vkEngine, SIGNAL(requestFinished(VKRequest *)),
            this, SLOT(audioGetFinished(VKRequest *)));
    connect(m_vkEngine, SIGNAL(requestFinished(VKRequest *)),
            this, SLOT(receivedAudio_add(VKRequest *)));
    connect(m_vkEngine, SIGNAL(requestFinished(VKRequest *)),
            this, SLOT(receivedDeleteMessage(VKRequest *)));

    //обработка тела сообщения
    bodyStyler = new MessageStyler(m_vkEngine);
    connect(bodyStyler,SIGNAL(stylingComplete(VKMessage*)),
            this,SLOT(messageStyled(VKMessage*)));

    //видео
    videoModel = new VideoListModel(m_vkEngine,ui->lvVideo,this);
    connect(videoModel, SIGNAL(selected()),this,SLOT(videoModelItemsReceived()));

    QMenu *m_emotic_menu = new QMenu(ui->tbEmoticons);
    QWidgetAction *m_emoticon_action = new QWidgetAction(m_emotic_menu);
    ChatEmoticonMenu *m_emoticon_widget = new ChatEmoticonMenu(m_emotic_menu, bodyStyler->emoticons);
    m_emoticon_action->setDefaultWidget(m_emoticon_widget);
    m_emotic_menu->addAction(m_emoticon_action);
    ui->tbEmoticons->setMenu(m_emotic_menu);
    connect(m_emoticon_widget, SIGNAL(insertSmile(const QString &)), this,
            SLOT(insertEmoticon(const QString &)));

    ui->lblUploadInfo->show();

    hasNewMessages = false;

    sendFailTimer = new QTimer(this);
    sendFailTimer->setInterval(4000);
    sendFailTimer->setSingleShot(true);
    connect(sendFailTimer, SIGNAL(timeout()), this, SLOT(slotFailedToSendMessage()));

    //LPS Online/Offline users
    connect(m_vkEngine,SIGNAL(lps8UserOnline(QString,QString)),
            this,SLOT(slotLps8UserOnline(QString,QString)));
    connect(m_vkEngine,SIGNAL(lps9UserOfline(QString,QString)),
            this,SLOT(slotLps9UserOffline(QString,QString)));

    reqGetProfilesId = m_vkEngine->reqGetProfiles(QStringList() << fromId,
                                                  QStringList() << "first_name" << "last_name"
                                                  << "photo_rec");
}

PersonalChat::~PersonalChat()
{
    delete ui;
}

void PersonalChat::slotProfilesLoaded(VKRequest *req)
{
    if (reqGetProfilesId!=req->reqId ||
        req->reqType != VKRequest::getProfiles){
        return;
    }

    QDomDocument doc("Profile");
    doc.setContent(req->result);
    QDomElement docElem = doc.documentElement();
    QDomElement fromIdElem = docElem.elementsByTagName ( "user" ).at(0).toElement();
    QString first_name = fromIdElem.elementsByTagName("first_name").at(0).toElement().text();
    QString last_name = fromIdElem.elementsByTagName("last_name").at(0).toElement().text();
    QString photo = fromIdElem.elementsByTagName("photo_rec").at(0).toElement().text();
    fromFullName = first_name + " " + last_name;
    fromFirstName = first_name;
    fromLastName = last_name;
    fromPhotoUrl = photo;

    m_Chats->chatSetTabText(fromId, fromFullName);

    midFirstName = m_vkEngine->m_rosterWindow->first_name;
    midLastName = m_vkEngine->m_rosterWindow->last_name;
    midFullName = midFirstName + " " + midLastName;

    isProfilesLoaded = true;
    //m_vkEngine->reqMessages_getHistory(fromId,"0","3");
    historyPartCount = 2;
    connect(webView,SIGNAL(linkClicked(QUrl)),
            this,SLOT(startDownloadNextPartOfHistory(QUrl)));
    startDownloadNextPartOfHistory(QUrl("getHistoryLink"));
    historyPartCount = 10;

    //фотки пользователя
    ui->dvPhotos->rootContext()->setContextProperty("photosModel", albumsModel);
    photoViewer = new PhotoViewer(ui->dvPhotoViewer, this);
}

void PersonalChat::messageReceived(VKMessage *incommingMessage)
{
    //m_chatsWindows->startTrayInc(incommingMessage->from_id);

    hasNewMessages = true;

    connect(incommingMessage, SIGNAL(changed(VKMessage*)),
            this, SLOT(messageChanged(VKMessage*)));

    bodyStyler->makeStyle(incommingMessage);
}

void PersonalChat::messageStyled(VKMessage *incommingMessage)
{
    if(!chatHistory.isEmpty()){
        if (chatHistory.last()->local_from_id != incommingMessage->local_from_id){
            incommingMessage->isFirst = true;
        }
    }else{
        incommingMessage->isFirst = true;
    }
    chatHistory.append(incommingMessage);

    unreadedFromIdMids.append(incommingMessage->local_id);

    if (isProfilesLoaded){

        QWebElement tbodyElement = webView->page()->mainFrame()->findFirstElement ( "[id=messagestbody]" );

        QString message = makeHtmlMess(incommingMessage);

        tbodyElement.appendInside(message);
    }

    if ((incommingMessage->flags & 2) && (!unreadedFromIdMids.isEmpty())){
        ui->lblUploadInfo->hide();
        markFromIdMessagesAsReadStart();
    }

    viewScroller->start();
}

void PersonalChat::startMigalko()
{
    incMessageMigalko->start();
}

void PersonalChat::stopMigalko()
{
    if (incMessageMigalko->isActive()){
        incMessageMigalko->stop();

        m_Chats->chatSetTabIcon(fromId, sMn->online);
        onlineMess = false;
        hasNewMessages = false;
    }
}

void PersonalChat::showEvent(QShowEvent *e)
{
    stopMigalko();
    QWidget::showEvent(e);
}

void PersonalChat::showChat()
{
    //если юзер онлайн оффлайн
    m_Chats->chatSetTabText(fromId, fromFullName);
    showed = true;
    show();
}

void PersonalChat::hideChat ( )
{
    #ifdef QT_PHONON_LIB
    pagePlayer->m_MediaObject.stop();
#endif
    showed = false;
    hide();
}

void PersonalChat::currentChatChanged(int id)
{
    chatEdit->setFocus();
    //stopMigalko();
}

void PersonalChat::messageChanged(VKMessage *mess)
{
    QString local_id = mess->local_id;
    int pos = -1;
    for(int i=0; i<chatHistory.count(); i++){
        VKMessage *histMess = chatHistory.at(i);
        if (histMess->local_id == local_id){
            pos = i;
            break;
        }
    }
    if (pos!=-1){
        chatHistory.removeAt(pos);
        chatHistory.insert(pos, mess);
    }

    QWebElement messageElement = webView->page()->mainFrame()->
                                 findFirstElement ( "[id=id"+ local_id +"]" );

    QString message = makeHtmlMess(mess);

    messageElement.replace(message);
}

void PersonalChat::updateView()
{
    #ifdef QT_PHONON_LIB
    pagePlayer->clearButtons();
    #endif

    QString html = sMn->sheader;

    QStringList messages;

    html.replace("_GETHISTORYLINK_",
                 tr("get more %1 (in all %2) messages...").arg(historyPartCount).arg(historyAllCount));

    foreach(VKMessage *mess, chatHistory){
        messages.append(makeHtmlMess(mess));
    }

    html += messages.join("");
    html += sMn->sfooter;
    webView->setHtml(html);

    //qDebug() << html;
}

QString PersonalChat::makeHtmlMess(VKMessage *mess)
{
    QString htmlMess = sMn->smessage;
    QString style;

    htmlMess.replace("_MARKASREADALT_", tr("Mark this message as read"));
    htmlMess.replace("_DELETEMSGALT_", tr("Delete this message"));
    htmlMess.replace("_DELETEMSGSRC_", QUrl::fromLocalFile(sMn->deletemsgpath).toString());
    htmlMess.replace("_MARKASREADSRC_", QUrl::fromLocalFile(sMn->markasreadpath).toString());
    htmlMess.replace("_MARKASREADURL_", "Messages_markAsReadManual_mid_"+mess->local_id);
    htmlMess.replace("_DELETEMSGURL_", "Messages_delete_mid_"+mess->local_id);
    //htmlMess.replace("_DELETEMSGURL_", "Messages_delete_mid_"+mess->message_id);
    htmlMess.replace("_QUOTEICONSRC_", QUrl::fromLocalFile(sMn->quotepath).toString());
    htmlMess.replace("_POSTEDBYTEXT_", tr("message from"));
    htmlMess.replace("_AVATARALT_", tr("Open user page in default browser"));

    if ( mess->flags & 2 ){
        //исходящее
        htmlMess.replace("_WHOSTYLE_", "out");
        htmlMess.replace("_USERNAME_", midFirstName);
        htmlMess.replace("_MARKASREADDISPLAY_", "none");
        htmlMess.replace("_AVATARSRC_", m_vkEngine->m_rosterWindow->photo_rec_url);
        htmlMess.replace("_AVATARURL_", "http://vkontakte.ru/id"+m_vkEngine->mid);
    }else{
        //входящее
        htmlMess.replace("_WHOSTYLE_", "in");
        htmlMess.replace("_USERNAME_", fromFirstName);
        htmlMess.replace("_AVATARSRC_", fromPhotoUrl);
        htmlMess.replace("_AVATARURL_", "http://vkontakte.ru/id"+fromId);
    }

    if (mess->isFirst){
        style += "message_first ";
    }

    style += "message ";

    if ( (mess->flags & 1) ){
        style += "message_unread";
        if (!( mess->flags & 2)){
            htmlMess.replace("_MARKASREADDISPLAY_", "inline");
        }
    }else{
        htmlMess.replace("_MARKASREADDISPLAY_", "none");
    }

    htmlMess.replace("_MESSAGE_LOCAL_ID_", mess->local_id);
    htmlMess.replace("_MESSAGESTYLE_", style);
    htmlMess.replace("_BODY_", mess->styledText);
    htmlMess.replace("_DATE_", mess->date);

    if (!mess->isDeleted()){
        htmlMess.replace("_MSGDELETED_", tr(""));
        htmlMess.replace("_DELETEMSGDISPLAY_", "inline");
    }else{
        htmlMess.replace("_MSGDELETED_", tr("deleted"));
        htmlMess.replace("_DELETEMSGDISPLAY_", "none");
    }

    return htmlMess;
}

void PersonalChat::onIncTimer()
{
    if ( !onlineMess ){
        m_Chats->chatSetTabIcon(fromId, sMn->incomming);
        onlineMess = true;
    }else{
        m_Chats->chatSetTabIcon(fromId, sMn->online);
        onlineMess = false;
    }
}

void PersonalChat::focusInEvent(QFocusEvent *e)
{
    stopMigalko();
    QWidget::focusInEvent( e );
}

//функция позволяет нам с задержкой в пару-тройку секунд
//пометить все пришедшие от собеседника сообщения как прочитанные
//задержка нужна чтобы пользователь мог сначала прочитать
//сообщения и поянть что из них новое а что нет.
//вызывается после того как читатель написал ответ.
//в таком случае получается, что все последние сообщения опонента
//будут отличаться по цвету, а все предыдущие уже будут такими же...
//ну или другими зависит от стиля.
void PersonalChat::markFromIdMessagesAsReadStart()
{
    if (!unreadedFromIdMids.isEmpty()){
        markFromIdMessagesAsReadTimer->start();
    }
}
//а вот здесь собственно шлём запрос на пометку некоего сообщения как прочитанного.
void PersonalChat::markFromIdMessagesAsReadFinish()
{
    markResponsReqId = m_vkEngine->reqMessages_markAsRead(unreadedFromIdMids);

    unreadedFromIdMids.clear();
}
//пустой обработчик markResponsReqId
void PersonalChat::markResponsReceived(VKRequest *req)
{
    if (req->reqType != VKRequest::Messages_markAsRead ||
        req->reqId != markResponsReqId){
        return;
    }
    //qDebug() << req->result;
}
//обрабатываем все клики по ссылкам в окошке чата.
void PersonalChat::urlClicked(QUrl url)
{
    if (url.toString().contains("Audio_add")){
        QStringList info = url.toString().split("_");
        ui->lblUploadInfo->show();
        reqAudioAddId = m_vkEngine->reqAudio_add(info.at(3), info.at(2));
        reqAudioAddAid = info.at(3);
        return;
    } else if (url.toString().contains("Messages_markAsReadManual")){
        QString mid = url.toString().split("_mid_").at(1);
        markResponsReqId = m_vkEngine->reqMessages_markAsRead(QStringList() << mid);
        return;
    }else if ( url.toString().contains("Messages_delete") ){
        QString mid = url.toString().split("_mid_").at(1);
        reqMessagesDeleteId = m_vkEngine->reqMessages_delete(mid);
        //qDebug() << mid;
        return;
    }else if (url.toString().contains("Images_urlbig")){
        ui->stackedWidget->setCurrentWidget(ui->pagePhotoViewer);
        photoViewer->showPhoto(url.toString().split("_urlbig_").at(1));
        return;
    }
    //qDebug() << url;
    QDesktopServices::openUrl ( url );
}
//пока обработчик с дебагом... непонятно почему сообщения не удаляются.
void PersonalChat::receivedDeleteMessage(VKRequest *req)
{
    if (req->reqType != VKRequest::Messages_delete ||
        req->reqId != reqMessagesDeleteId){
        return;
    }

    //хз какого хрена оно не работает =(
    qDebug() << req->result;
}
//собеседник кликнул по ссылке "добавить аудио",
//отправляем сообщение и убираем эту ссылку в окошке чата.
void PersonalChat::receivedAudio_add(VKRequest *req)
{
    if (req->reqType != VKRequest::Audio_add ||
        req->reqId != reqAudioAddId){
        return;
    }

    ui->lblUploadInfo->hide();
    QWebElement audioElem = webView->page()->mainFrame()->findFirstElement("[id=id"+reqAudioAddAid+"]");
    audioElem.replace(" ");

    VKMessage *outMessage = new VKMessage( m_vkEngine );

    outMessage->to_id = fromId;

    if (req->result.contains("<response>1</response>")){
        outMessage->setText(tr("Audio is added to my page"));
        outMessage->subject = tr("Audio added");
    }else{
        outMessage->setText(tr("Unable to add this audio"));
        outMessage->subject = tr("Audio added failed");
    }

    outMessage->date = QTime::currentTime().toString("hh:mm:ss");
    reqSendId = m_vkEngine->reqMessages_send(outMessage);
}

//история. Отправляем запрос на получение куска истории.
void PersonalChat::startDownloadNextPartOfHistory(QUrl url)
{
    if (url.toString() != "getHistoryLink"){
        urlClicked(url);
        return;
    }

    ui->lblUploadInfo->show();

    getHistoryId = m_vkEngine->reqMessages_getHistory(fromId,
                                                      QString::number(chatHistory.count()),
                                                      QString::number(historyPartCount));
}
//история. Получили кусок и обрабюатываем его.
void PersonalChat::getHistoryFinished(VKRequest *req)
{
    if (req->reqType != VKRequest::Messages_getHistory ||
        req->reqId != getHistoryId){
        return;
    }
    getHistoryId = -1;

    QDomDocument doc("History");
    doc.setContent(req->result);
    QDomElement docElem = doc.documentElement();

    if (docElem.elementsByTagName("response").at(0).toElement().text()=="0"){
        updateView();
        qDebug() << "not more messages! Oblomis =))";
        return;
    }

    historyAllCount = docElem.elementsByTagName("count").at(0).toElement().text();

    QDomNodeList messages = docElem.elementsByTagName ( "message" );
    for(int i=0; i < messages.count(); i++){
        QDomElement messageElem = messages.at(i).toElement();
        int flags = 0;

        VKMessage *mess = new VKMessage(m_vkEngine);

        mess->setText(messageElem.elementsByTagName("body").at(0).toElement().text());
        mess->local_id = messageElem.elementsByTagName("mid").at(0).toElement().text();

        mess->from_id = messageElem.elementsByTagName("from_id").at(0).toElement().text();
        mess->local_from_id = messageElem.elementsByTagName("from_id").at(0).toElement().text();
        if (mess->from_id == m_vkEngine->mid){
            flags += 2;
        }
        mess->date = QDateTime::fromTime_t(messageElem.elementsByTagName("date").at(0).toElement().text().toInt()).toString("dd.MM.yyyy hh:mm:ss");
        int read_state = messageElem.elementsByTagName("read_state").at(0).toElement().text().toInt();
        if (read_state != 1){
            flags += 1;
        }
        mess->local_id = messageElem.elementsByTagName("mid").at(0).toElement().text();

        mess->setFlags(flags);
        mess->fromHistory = true;

        connect(mess, SIGNAL(changed(VKMessage*)),
                this, SLOT(messageChanged(VKMessage*)));

        bodyStyler->makeFastStyle(mess);

        if (messages.count() > i){
            if (messages.at(i+1).toElement().elementsByTagName("from_id").at(0).toElement().text() != mess->local_from_id ) {
                mess->isFirst = true;
            }
        }else{
            mess->isFirst = true;
        }

        chatHistory.insert(0,mess);
    }

    updateView();

    ui->lblUploadInfo->hide();

    webView->page()->mainFrame()->setScrollBarValue(Qt::Vertical, 0);

}
//конец истории

//возврат со странички добавления аудио
void PersonalChat::on_pbCancelPostAudio_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageChat);
    ui->lblUploadAudioInfo->hide();
}
//открываем страничку выбора аудио для отправки и подгружаем это самое аудио.
void PersonalChat::on_tbAddAudio_clicked()
{
    audioItems->clear();
    ui->stackedWidget->setCurrentWidget(ui->pagePostAudio);
    ui->lblUploadAudioInfo->show();
    getAudioId = m_vkEngine->reqAudio_get(m_vkEngine->mid);
}
//получили список песенок, загружаем их в модель.
void PersonalChat::audioGetFinished(VKRequest *req)
{
    if (req->reqType != VKRequest::Audio_get ||
        req->reqId != getAudioId){
        return;
    }

    audioItems->clear();

    QDomDocument doc("Audio");
    doc.setContent(req->result);
    QDomElement docElem = doc.documentElement();
    QDomNodeList audios = docElem.elementsByTagName ( "audio" );
    for(int i=0; i < audios.count(); i++){
        QDomElement audioElem = audios.at(i).toElement();
        QStandardItem *aid = new QStandardItem(audioElem.elementsByTagName("aid").at(0).toElement().text());
        QStandardItem *owner_id = new QStandardItem(audioElem.elementsByTagName("owner_id").at(0).toElement().text());
        QTime time(0,0,0);
        QTime durTime = time.addSecs(audioElem.elementsByTagName("duration").at(0).toElement().text().toInt());
        QStandardItem *description = new QStandardItem(audioElem.elementsByTagName("artist").at(0).toElement().text() + " - " +
                                                       audioElem.elementsByTagName("title").at(0).toElement().text()+ " ( " +
                                                       durTime.toString("mm:ss") + " ) ");
        QStandardItem *url = new QStandardItem(audioElem.elementsByTagName("url").at(0).toElement().text());
        audioItems->appendRow(QList<QStandardItem *>() << description << aid << owner_id << url);
    }
    ui->lblUploadAudioInfo->hide();
}
//нажата кнопка отправки аудио - посылаем сообщение...
void PersonalChat::on_pbAudioSend_clicked()
{
    if (!ui->lvAudio->currentIndex().isValid()){
        return;
    }

    ui->lblUploadInfo->show();
    disableEmlements();
    sendFailTimer->start();

    VKMessage *outMessage = new VKMessage( m_vkEngine );

    QString desc = audioItems->item(ui->lvAudio->currentIndex().row(),0)->text();
    QString aid = audioItems->item(ui->lvAudio->currentIndex().row(),1)->text();
    QString owner_id = audioItems->item(ui->lvAudio->currentIndex().row(),2)->text();
    QString url = audioItems->item(ui->lvAudio->currentIndex().row(),3)->text();

    outMessage->to_id = fromId;
    outMessage->setText(tr("Audio: ") + desc.trimmed() + "<*>[[audio" + owner_id + "_" + aid + "]]");
    outMessage->subject = tr("Audio");
    outMessage->date = QTime::currentTime().toString("hh:mm:ss");
    outMessage->flags += 512;

    reqSendId = m_vkEngine->reqMessages_send(outMessage);

    ui->stackedWidget->setCurrentWidget(ui->pageChat);
}
//открываем страничку с альбомами и фотками.
void PersonalChat::on_tbAddPhoto_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageQmlPhotos);
    if (ui->dvPhotos->source().toString() != "qrc:/declarative/photoviewer/photoviewer.qml"){
        ui->dvPhotos->rootContext()->setContextProperty("fromId", QVariant(fromId));
        ui->dvPhotos->setSource(QUrl("qrc:/declarative/photoviewer/photoviewer.qml"));
    }
}
//отмена отправки аудио =))))
//смайлик потому чт оу меня анстроение хороше =)
void PersonalChat::on_pbCancelSendPhoto_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageChat);
}
//фотка выбрана... Ну точнее из qml пришёл сигнал нажатия по кнопке Send
//отправляем сообщение с фоткой
void PersonalChat::photoIsSelected(QString preview, QString hq,
                                   QString pid, QString aid, QString mid,
                                   QString fromId)
{
    if (fromId != this->fromId){
        return;
    }
    ui->stackedWidget->setCurrentWidget(ui->pageChat);

    //фотка выбрана - отправляем её =)
    ui->lblUploadInfo->show();
    disableEmlements();
    sendFailTimer->start();

    VKMessage *outMessage = new VKMessage( m_vkEngine );

    QString desc = tr("Photo (click for enlarge):");
    outMessage->to_id = fromId;
    outMessage->setText(desc.trimmed() + "<*>[[photo" + mid + "_" + pid + "]]");
    //desc + "_" + owner_id + "_" + aid + " " + url;
    outMessage->subject = tr("Photo");
    outMessage->date = QTime::currentTime().toString("hh:mm:ss");
    outMessage->flags += 512;

    reqSendId = m_vkEngine->reqMessages_send(outMessage);

    ui->stackedWidget->setCurrentWidget(ui->pageChat);
}
//скролл всегда вызывается с задержкой в несколько миллисекунд.
//ибо вебкит сцука страшная гонит и не сразу возвращает правильную высоту =)
void PersonalChat::scrollToBottom() {
    webView->page()->mainFrame()->setScrollBarValue(Qt::Vertical, webView->page()->mainFrame()->scrollBarMaximum(Qt::Vertical));
}
//ну тут жуже понятно - открываем страничку выбора видео для отправки
void PersonalChat::on_tbSendVideo_clicked()
{
    videoModel->select();
    ui->lblUploadVideoInfo->show();
    ui->stackedWidget->setCurrentWidget(ui->pagePostVideo);
}
//закончилась загрузка данных в модель видео
void PersonalChat::videoModelItemsReceived()
{
    ui->lblUploadVideoInfo->hide();
}
//прост открываем окошко чата и отменяем выбор видео для отправки
void PersonalChat::on_pbCancelPostVideo_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageChat);
}
//отправляем сообщение с видео.
void PersonalChat::on_pbSendVideo_clicked()
{
    if (!ui->lvVideo->currentIndex().isValid()){
        return;
    }

    ui->lblUploadInfo->show();
    disableEmlements();

    VKMessage *outMessage = new VKMessage( m_vkEngine );

    QString title = videoModel->item(ui->lvVideo->currentIndex().row(),2)->text();
    QString imgUrl = videoModel->item(ui->lvVideo->currentIndex().row(),3)->text();
    QString url = videoModel->item(ui->lvVideo->currentIndex().row(),1)->text();
    QString vid = videoModel->item(ui->lvVideo->currentIndex().row(),4)->text();

    outMessage->to_id = fromId;

    //пока отправляем просто ссылку на видео. В будущем всё будет по
    //нормальному когда из апи можно будет получить прямую ссылку на видео файл
    //то и проигрывать мы его будем прямо в программе.
    QString text = tr("Video: ") + title.trimmed()+ "<div id=\"videoLink\"><br><a href='" + url + "'><img src='" + imgUrl + "' /></a></div>";

    text += "<br/><*>[[video" + m_vkEngine->mid + "_" + vid + "]]";
    outMessage->setText(text);
    outMessage->subject = tr("Video");
    outMessage->date = QTime::currentTime().toString("hh:mm:ss");
    outMessage->flags += 512;

    //qDebug() << text;

    reqSendId = m_vkEngine->reqMessages_send(outMessage);

    ui->stackedWidget->setCurrentWidget(ui->pageChat);
}
//вставляем
void PersonalChat::insertEmoticon(const QString &str)
{
    chatEdit->insertPlainText(str);
    chatEdit->setFocus();
}
//что-то пошло не так и отправить собщение не получилось
//может сработать. если лонгпул сервер не уведомил о доставке
//либо есть какаие то проблемы с сетью...
//ну и даже не знаю после чего есчо
void PersonalChat::slotFailedToSendMessage()
{
    enableEmlements();
}
//блокируем элементы управления в чате.
void PersonalChat::disableEmlements()
{
    chatEdit->setEnabled(false);
    ui->tbEmoticons->setEnabled(false);
    ui->tbAddPhoto->setEnabled(false);
    ui->tbAddAudio->setEnabled(false);
    ui->tbSendVideo->setEnabled(false);
    ui->sendButton->setEnabled(false);
}
//разблокируем элементы управления
void PersonalChat::enableEmlements()
{
    chatEdit->setEnabled(true);
    ui->tbEmoticons->setEnabled(true);
    ui->tbAddPhoto->setEnabled(true);
    ui->tbAddAudio->setEnabled(true);
    ui->tbSendVideo->setEnabled(true);
    ui->sendButton->setEnabled(true);
    chatEdit->setFocus();
}
//отправляем то что накропали в редакторе сообщений собеседнику
void PersonalChat::on_sendButton_clicked()
{
    if (chatEdit->toPlainText().trimmed().isEmpty()){
        return;
    }

    ui->lblUploadInfo->show();
    disableEmlements();
    sendFailTimer->start();

    VKMessage *outMessage = new VKMessage( m_vkEngine );

    outMessage->to_id = fromId;
    outMessage->from_id = m_vkEngine->mid;

    //The special characters are $, (,), *, +, ., ?, [, ,], ^, {, | and }.
    //QString text = QRegExp::escape (chatEdit->toPlainText());
    //text = text.replace()

    //QString text = QUrl::toPercentEncoding(chatEdit->toPlainText(), "", "+");

    QString text = chatEdit->toPlainText();

    text = text.replace("+","kf_plus");

    //qDebug() << text;

    outMessage->setText(text);
    outMessage->subject = "";
    outMessage->date = QTime::currentTime().toString("hh:mm:ss");

    reqSendId = m_vkEngine->reqMessages_send(outMessage);

}
//отправленное сообщение вернулось, то бишь оно скорее всего доставилось =)
void PersonalChat::sendMessageReceived(VKRequest *req)
{
    if (req->reqType != VKRequest::Messages_send ||
        reqSendId != req->reqId){
        return;
    }
    reqSendId = -1;

    if (!req->result.contains("<error>")){
        chatEdit->clear();
        chatEdit->document()->allFormats().clear();
    }else{
        qDebug() << req->result;
    }

    sendFailTimer->stop();
    enableEmlements();
    ui->lblUploadInfo->hide();

    if (req->result.contains("Flood control enabled for this action") ||
        req->result.contains("<error_code>9</error_code>") ){

        VKMessage *outMessage = new VKMessage( m_vkEngine );

        outMessage->to_id = fromId;

        QString text = tr("Your message is flood!");

        outMessage->setText(text);
        outMessage->subject = tr("Flood Control");
        outMessage->date = QTime::currentTime().toString("hh:mm:ss");
        outMessage->flags += 512;

        reqSendId = m_vkEngine->reqMessages_send(outMessage);
        //ui->lblUploadInfo->hide();
    }
    if (req->result.contains("Permission to perform this action is denied by user") ||
        req->result.contains("<error_code>7</error_code>") ){
        chatEdit->append(tr("Error: Permission to perform this action is denied by user"));
    }

    chatEdit->setFocus();

}
//меняем иконку на соответствующую статусу юзера
void PersonalChat::slotLps8UserOnline(QString uid, QString flags)
{
    if (uid == fromId){
        m_Chats->chatSetTabIcon(fromId, sMn->online);
    }
}
//меняем иконку на соответствующую статусу юзера
void PersonalChat::slotLps9UserOffline(QString uid, QString flags)
{
    if (uid == fromId){
        m_Chats->chatSetTabIcon(fromId, sMn->offline);
    }
}
//посмотрели присланную картинку, возвращаемся в чат.
void PersonalChat::on_pbPVback_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageChat);
}

void PersonalChat::on_rosterButton_clicked()
{
    m_Chats->m_rosterWindow->slidingStacked->slideInIdx(m_Chats->m_rosterWindow->slidingStacked->indexOf(m_Chats->m_rosterWindow->rosterWidget));
}
