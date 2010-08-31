#include "vkengine.h"
#include <QHBoxLayout>
#include <QWebView>
#include <QWebPage>
#include <QWebFrame>
#include <QWebElement>
#include <QLineEdit>
#include <QDebug>
#include <QProgressBar>
#include "../vimkamain.h"
#include "../trayicon.h"
#include "../chats/chats.h"
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QTimer>
#include <QTime>
#include <QHttpResponseHeader>
#include <QHttp>
#include <QHttpRequestHeader>
#include "vkmessage.h"
#include <QMessageBox>

VKEngine::VKEngine(VimkaMain *rosterWindow, QObject *parent) :
        QObject(parent)
{
    m_rosterWindow = rosterWindow;
    isAppConnected = false;

    m_reqCounter = 0;

    app_id = "1914138";
    //http://vkontakte.ru/login.php?app=1914138&layout=popup&type=browser&settings=16383

//    Список прав:

//    Код	Описание
//    +1	Пользователь разрешил отправлять ему уведомления.
//    +2	Доступ к друзьям.
//    +4	Доступ к фотографиям.
//    +8	Доступ к аудиозаписям.
//    +16	Доступ к видеозаписям.
//    +32	Доступ к предложениям.
//    +64	Доступ к вопросам.
//    +128	Доступ к wiki-страницам.
//    +256	Добавление ссылки на приложение в меню слева.
//    +512	Добавление ссылки на приложение для быстрой публикации на стенах пользователей.
//    +1024	Доступ к статусам пользователя.
//    +2048	Доступ заметкам пользователя.
//    +4096	(для Desktop-приложений) Доступ к расширенным методам работы с сообщениями.
//    +8192	(для Desktop-приложений) Доступ к расширенным методам работы со стеной.

    int perms = 1 + 2 + 4 + 8 + 16 + 1024 + 4096 + 8192;

    authPageUrl = QUrl("http://vkontakte.ru/login.php?app="+app_id+"&layout=popup&type=browser&settings=" + QString::number(perms));
    // authPageUrl = QUrl("http://vkontakte.ru/login.php?app="+app_id+"&layout=popup&type=browser&settings=4095");

    app_key = "MnJ2xvehDeGnn8OnpjWw";
    api_url = "http://api.vkontakte.ru/api.php";

    viewBrowser = new QWidget();
    debugView = new QWebView();

}

//запрашиваем авторизацию и при необходимости настройки.
void VKEngine::requestAppAuth(QString userMail, QString userPass)
{
    //
    viewBrowser = new QWidget();
    viewBrowser->setGeometry(0,0,300,400);
    QVBoxLayout *l = new QVBoxLayout(viewBrowser);

    debugView = new QWebView();
    l->addWidget(debugView);
    connect(debugView->page(),SIGNAL(loadFinished(bool)),this,SLOT(authPageLoaded(bool)));
    connect(debugView->page(),SIGNAL(loadFinished(bool)),this,SLOT(videoUrlReceived()));
    httpReqType = Login;

    m_reqHttp = new QHttp("api.vkontakte.ru", 80, this);

    reqInterval = new QTime();
    reqInterval->start();

    loginTimer = new QTimer(this);
    loginTimer->setInterval(3000);
    loginTimer->setSingleShot(true);

    authPageTimer = new QTimer(this);
    authPageTimer->setInterval(30000);
    authPageTimer->setSingleShot(true);

    connect (m_reqHttp, SIGNAL( done ( bool ) ), this, SLOT( slotReqFinished(bool)));
    connect (this,SIGNAL(connected()),this,SLOT(slotConnected()));
    connect (this,SIGNAL(requestFinished(VKRequest*)),this,SLOT( slotLongPollServerReceived(VKRequest*)));
    connect (loginTimer,SIGNAL(timeout()),this, SLOT(slotShowLoginPage()));
    connect (authPageTimer,SIGNAL(timeout()),this, SLOT(slotAuthPageFailedLoad()));

    m_requestInAction = false;
    //

    m_rosterWindow->trayIcon->setIcon(TrayIcon::Connecting);
    m_user = userMail;
    m_pass = userPass;

    loggined = false;

    debugView->load(authPageUrl);
    authPageTimer->start();
}
//удаляем всё лишнее.
void VKEngine::disconnect()
{
    QObject::disconnect (m_reqHttp, SIGNAL( done ( bool ) ), this, SLOT( slotReqFinished(bool)));
    QObject::disconnect (debugView->page(),SIGNAL(loadFinished(bool)),this,SLOT(authPageLoaded(bool)));
    QObject::disconnect(this,SIGNAL(requestFinished(VKRequest*)),this,SLOT( slotLongPollServerReceived(VKRequest*)));
    QObject::disconnect(this,SIGNAL(connected()),this,SLOT(slotConnected()));
    QObject::disconnect(debugView->page(),SIGNAL(loadFinished(bool)),this,SLOT(authPageLoaded(bool)));
    QObject::disconnect(loginTimer,SIGNAL(timeout()),this, SLOT(slotShowLoginPage()));

    //delete viewBrowser;

    debugView->setUrl(QUrl("about:blank"));
    loginTimer->stop();
    authPageTimer->stop();
    loggined = false;
    requests.clear();
    reqInterval->restart();
    m_rosterWindow->trayIcon->setIcon(TrayIcon::Offline);
    isAppConnected = false;
    emit disconnected();
}
//загружена страничка авторизации, посдавляем логин и пароль и затем показываем список
//галочек с требуемыми правами и кнопку соглашения с ними.
void VKEngine::authPageLoaded(bool ok)
{
    if (httpReqType != Login){
        return;
    }

    QString url = debugView->url().toString();
    QWebElement document = debugView->page()->mainFrame()->documentElement();

    if (url == "about:blank"){
        return;
    }

    if (url.contains("http://vkontakte.ru/api/login_failure.html")){
        m_rosterWindow->chats->hideInfoPage("Login info");
        m_rosterWindow->trayIcon->setIcon(TrayIcon::Offline);
        disconnect();
    }

    //qDebug() << url;
    if ( url.contains("http://vkontakte.ru/api/login_success.html#session=")){

        //QObject::disconnect(debugView->page(),SIGNAL(loadFinished(bool)),this,SLOT(authPageLoaded(bool)));
        //connect(debugView->page(),SIGNAL(loadFinished(bool)),this,SLOT(slotReqFinished(bool)));
        url.remove("http://vkontakte.ru/api/login_success.html#session=");
        url.remove("{");
        url.remove("}");
        QStringList jsonList = url.split(",");
        foreach(QString elem, jsonList){
            QString key = elem.split(":").at(0);
            key = key.remove("\"");
            QString value = elem.split(":").at(1);
            value = value.remove("\"");

            if (key == "expire"){
                expire = value;
            }
            if (key == "mid"){
                mid = value;
            }
            if (key == "secret"){
                secret = value;
            }
            if (key == "sid"){
                sid = value;
            }
        }
        emit connected();
    }

    if (loggined==true){

        //QWebElement emailTextInput = document.findFirst("input[type=email]");
        //QWebElement passTextInput = document.findFirst("input[type=password]");

        //m_rosterWindow->currentEmail = m_rosterWindow->lastEmail = emailTextInput.attribute("value");
        //m_rosterWindow->currentPass = m_rosterWindow->lastPass =  passTextInput.attribute("value");

        //закомментировано чтобы юзер мог увидеть какие галки он собирается выставить.
        document.evaluateJavaScript("saveSettings()");
    }else{
        //<input type="email" onkeydown="if (event.keyCode == 10 || event.keyCode == 13) doLogin();" class="inputText" name="email" id="email">

        QWebElement emailTextInput = document.findFirst("input[type=email]");
        emailTextInput.setAttribute("value", m_user);

        //<input type="password" onkeydown="if (event.keyCode == 10 || event.keyCode == 13) doLogin();" class="inputText" autocomplete="off" id="pass">

        QWebElement passTextInput = document.findFirst("input[type=password]");
        passTextInput.setAttribute("value", m_pass);

        loggined = true;
        document.evaluateJavaScript("doLogin()");
        authPageTimer->stop();
        loginTimer->start();
    }
}
//подключились - инициализируем lps сервер
void VKEngine::slotConnected()
{
    //qDebug() << "Connected at "+QTime::currentTime().toString("hh:mm:ss.zzz");
    isAppConnected = true;
    loginTimer->stop();
    m_rosterWindow->chats->hideInfoPage("Login info");
    m_rosterWindow->trayIcon->setIcon(TrayIcon::Online);

    lpsView = new QWebView(viewBrowser);
    connect(lpsView->page(),SIGNAL(loadFinished(bool)),this,SLOT(slotLPSRequestReceived(bool)));

    //начинаем цикл работы нашего LPS
    reqIdLPS = reqMessages_getLongPollServer();
    //reqIdLPS = reqMessages_getLongPollServer();
    //reqIdLPS = reqMessages_getLongPollServer();
}
//показываем страничку логина
void VKEngine::slotShowLoginPage()
{
    m_rosterWindow->chats->openInfoPage("Login info", viewBrowser);
    m_rosterWindow->chats->showInfoPage("Login info");
}
//не удалось загрузить страничку логина - значит нет интернета и подключиться не получится
void VKEngine::slotAuthPageFailedLoad()
{
    QMessageBox::information(m_rosterWindow,tr(""),tr("Unable to connect!"));
}
//некий запрос из очереди запросов выполнился
void VKEngine::slotReqFinished(bool ok)
{
    QString result = m_reqHttp->readAll();
    //qDebug() << m_currentRequest->request;
    //qDebug() << "RESULT: " <<  m_currentRequest->requestTypeText << result;

    //qDebug()<< req->reqId << reqIdLPS << req->requestTypeText;

    m_currentRequest->result = result;

    m_requestInAction = false;
    emit requestFinished(m_currentRequest);

    if (requests.isEmpty()){
        m_requestInAction = false;
    }else{
        execNextRequest();
    }
}
//выполняем следующий запрос в списке запросов
void VKEngine::execNextRequest()
{

    if (!isAppConnected){
        return;
    }
    if (reqInterval->elapsed()<350 || m_requestInAction){
        QTimer *timer = new QTimer();
        timer->setSingleShot(true);
        timer->setInterval(350);
        connect(timer, SIGNAL(timeout()), this, SLOT(execNextRequest()));
        timer->start();
        return;
    }

    m_requestInAction = true;

    if (!requests.isEmpty()){
        m_currentRequest = requests.takeFirst();
    }

    //qDebug() << m_currentRequest->requestTypeText;

    //qDebug() << "REQUEST: " << m_currentRequest->requestTypeText << m_currentRequest->requestUrlPath;

    QByteArray content = "";
    content.append(m_currentRequest->requestUrlPath);

    QHttpRequestHeader header("POST", "/api.php");
    header.setValue("Host", "api.vkontakte.ru");
    header.setContentType("application/x-www-form-urlencoded");
    header.setContentLength(content.length());

    //qDebug() << header.toString() << content;

    reqInterval->restart();
    m_reqHttp->request(header, content);

}
//добавляем новый запрос в список
void VKEngine::appendRequest(VKRequest *req)
{
    requests.append(req);
    if (!m_requestInAction){
        execNextRequest();
    }
}

//обработка первичной информации необходимой для подключения к longpoll серверу.
void VKEngine::slotLongPollServerReceived(VKRequest *req)
{
    //qDebug() << req->result;

    if((req->reqType!=VKRequest::Messages_getLongPollServer) ||
       (reqIdLPS != req->reqId)){
        return;
    }

    //qDebug() << req->result;

    QDomDocument doc("LPS");
    doc.setContent(req->result);
    QDomElement docElem = doc.documentElement();

    QDomNode n = docElem.firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if(!e.isNull()) {
            if (e.tagName()=="key"){
                keyLPS = e.text();
            }
            if (e.tagName()=="server"){
                serverLPS = e.text();
            }
            if (e.tagName()=="ts"){
                tsLPS = e.text();
            }
            //qDebug() << qPrintable(e.tagName()) << qPrintable(e.text()); // the node really is an element.
        }
        n = n.nextSibling();
    }

    //waitLPS = "6";
    waitLPS = "25";
    QString urlLPS =  "http://"+serverLPS+"?act=a_check&key="+keyLPS+"&ts="+tsLPS+"&wait="+waitLPS;

    //qDebug() << "LPS Connected. First URL: " << urlLPS;

    lpsView->load(urlLPS);

}
//получено событие пришедше от  lps сервера
void VKEngine::slotLPSRequestReceived(bool ok)
{
    if (!isAppConnected){
        return;
    }

    if (lpsView->url().toString() == "about:blank"){
        return;
    }

    //qDebug() << req

    //для теста на мпотом
    //http://im1.vkontakte.ru/im027?act=a_check&key=24ce6a5ccd41e725a4e1e41a28784c43dd9e6f80&ts=726333264&wait=25

    lpsResult = lpsView->page()->mainFrame()->toPlainText();
    //qDebug() << lpsResult;

    //с первого раза может не получиться подключиться к лонгпул серверу либо он может вернуть какую то фигню
    if (lpsResult.contains("failed")){
        lpsView->load(QUrl("about:blank"));
        qDebug() << "Failed get TS! Update LPS connection";
        reqIdLPS = reqMessages_getLongPollServer();
        return;
    }

    lpsResult.remove("{");
    lpsResult.remove("}");

    //ts parsing
    QString tsLPS = lpsResult.left(lpsResult.indexOf(",")).remove("ts").remove(":").remove("\"").trimmed();
    //lpsResult = lpsResult.right;
    //qDebug() << tsLPS;

    //updates
    QString updates = lpsResult.left(lpsResult.indexOf("\"updates\":"));

    updates = lpsResult.remove(updates+"\"updates\":").trimmed();

    //начинаем обрабатывать массив updates
    if (updates.startsWith("[")){
        updates = updates.remove ( 0, 1 );
    }
    if (updates.endsWith("]")){
        updates = updates.remove ( updates.count()-1, 1 );
    }
    //qDebug() << "UPDATES: " << updates;
    QStringList updatesUnresolvedList = updates.split("],[");
    QStringList updatesResolvedList;
    foreach(QString item, updatesUnresolvedList){
        if (item.startsWith("[")){
            item = item.remove ( 0, 1 );
        }
        if (item.endsWith("]")){
            item = item.remove ( item.count()-1, 1 );
        }
        updatesResolvedList.append(item);
    }

    if (updatesResolvedList.count()==0 || updatesResolvedList.at(0).isEmpty()){
        QString urlLPS =  "http://"+serverLPS+"?act=a_check&key="+keyLPS+"&ts="+tsLPS+"&wait="+waitLPS;
        lpsView->load(urlLPS);
        return;
    }

    foreach(QString update, updatesResolvedList){
        QString codeText,local_id = "", user_id = "";
        int flags, mask;
        QString code = update.left(update.indexOf(","));
        update = update.remove(0, update.indexOf(",")+1);
        VKMessage *message;
        switch(code.toInt()){
        case 0: //0,$local_id,0 -- удаление сообщения с указанным local_id
            local_id = update.left(update.indexOf(","));
            codeText = "delete local_id message";
            break;
        case 1: //1,$local_id,$flags -- замена флагов сообщения (FLAGS:=$flags)
            local_id = update.left(update.indexOf(","));
            update = update.remove(0, update.indexOf(","));
            flags = update.remove(",").toInt();
            codeText = "replace message "+local_id+" flags  (FLAGS:=$flags) Flags: "+QString::number(mask);
            emit lps0DeleteMessage(local_id);
            break;
        case 2: //2,$local_id,$mask -- установка флагов сообщения (FLAGS|=$mask)
            local_id = update.left(update.indexOf(","));
            update = update.remove(0, update.indexOf(","));
            mask =  update.left(update.indexOf(",")).toInt();
            update = update.remove(0, update.indexOf(",")+1);
            flags = update.remove(",").toInt();
            codeText = "set message "+local_id+" flags  (FLAGS|=$mask) Mask: "+QString::number(mask);
            emit lps2SetMessageFlags(local_id, mask);
            break;
        case 3: //3,$local_id,$mask,$user_id -- сброс флагов сообщения (FLAGS&=~$mask)
            //qDebug () << lpsResult;
            //qDebug() << update;
            local_id = update.left(update.indexOf(","));
            //qDebug() << update;
            update = update.remove(0, update.indexOf(",")+1);
            //qDebug() << update;
            mask =  update.left(update.indexOf(",")).toInt();
            update = update.remove(0, update.indexOf(",")+1);
            //qDebug() << update;
            user_id = update.remove(",").toInt();
            //qDebug() << update;
            codeText = "reset message "+local_id+" flags  (FLAGS&=~$mask) Mask: "+QString::number(mask);
            emit lps3ResetMessageFlags(local_id, mask);
            break;
        case 4://4,$local_id,$flags,$from_id,$message_id,$subject,$text -- добавление нового сообщения
            message = new VKMessage( this );

            message->local_id = local_id = update.left(update.indexOf(","));
            update = update.remove(0, update.indexOf(",")+1);
            flags = update.left(update.indexOf(",")).toInt();
            message->setFlags(flags);
            update = update.remove(0, update.indexOf(",")+1);
            message->from_id = update.left(update.indexOf(","));
            if (message->flags & 2){
                message->local_from_id = mid;
            }else{
                message->local_from_id = update.left(update.indexOf(","));
            }
            //qDebug() << update;
            update = update.remove(0, update.indexOf(",")+1);
            //qDebug() << update;
            message->message_id = update.left(update.indexOf(","));
            //qDebug() << message->message_id;
            update = update.remove(0, update.indexOf(",")+1);
            message->subject = update.left(update.indexOf(","));
            update = update.remove(0, update.indexOf(",")+1);
            update = update.remove(0,1);
            message->setText(update.remove(update.count()-1,1));
            //qDebug() << message->getText();
            codeText = "received new message local_id="+message->local_id+
                       " flags="+QString::number(message->flags)+" from_id="+message->from_id+
                       " message_id="+message->message_id+
                       " subject="+message->subject+
                       " text="+message->getText();
            message->date = QTime::currentTime().toString("hh:mm:ss");
            emit lps4MessageReceived(message);
            break;
        case 8://8,-$user_id,0 -- друг $user_id стал онлайн
            user_id = update.left(update.indexOf(","));
            update = update.remove(0, update.indexOf(","));
            flags = update.remove(",").toInt();
            codeText = "friend "+user_id+" is online. Flags: "+flags;
            emit lps8UserOnline(user_id.remove("-"), QString::number(flags));
            break;
        case 9://9,-$user_id,$flags -- друг $user_id стал оффлайн ($flags равен 0, если пользователь покинул сайт (например, нажал выход) и 1, если оффлайн по таймауту (например, статус away))
            user_id = update.left(update.indexOf(","));
            update = update.remove(0, update.indexOf(","));
            flags = update.remove(",").toInt();
            codeText = "friend "+user_id+" is offline. Flags: "+flags;
            emit lps9UserOfline(user_id.remove("-"),  QString::number(flags));
            break;
        }

        //qDebug() << code << codeText;

    }

    //qDebug() << "UPDATES: " << updatesResolvedList;


    //next req
    QString urlLPS =  "http://"+serverLPS+"?act=a_check&key="+keyLPS+"&ts="+tsLPS+"&wait="+waitLPS;
    lpsView->load(urlLPS);
    /*
Первым параметром каждого события передаётся его код, поддерживаются следующие коды событий:

0,$local_id,0 -- удаление сообщения с указанным local_id
1,$local_id,$flags -- замена флагов сообщения (FLAGS:=$flags)
2,$local_id,$mask -- установка флагов сообщения (FLAGS|=$mask)
3,$local_id,$mask -- сброс флагов сообщения (FLAGS&=~$mask)
4,$local_id,$flags,$from_id,$message_id,$subject,$text -- добавление нового сообщения
8,-$user_id,0 -- друг $user_id стал онлайн
9,-$user_id,$flags -- друг $user_id стал оффлайн ($flags равен 0, если пользователь покинул сайт (например, нажал выход) и 1, если оффлайн по таймауту (например, статус away))

Пример ответа с событием:
{ ts: 196851352, updates: [ [ 9, -835293, 1 ], [ 9, -23498, 1 ] ] }

Это значит что пользователи 835293 и 23498 покинули сайт.

Пример ответа с сообщением:
{ ts: 196851367, updates: [ [ 4, 16929, 1, 85635407, 1280307577, ' ... ', 'hello' ] ] }

Время действия ключа для подключения к LongPoll серверу может истечь через некоторое время, сервер вернёт параметр failed:
{ failed: 2 }

 в таком случае требуется перезапросить его используя метод messages.getLongPollServer.
*/
}

/*
 * список всех используемых методов
 *
 */

//пользователи
int VKEngine::reqGetProfiles(QStringList uids, QStringList fields)
{
    int id = m_reqCounter++;
    VKRequest *request = new VKRequest(VKRequest::getProfiles, mid, sid, secret, app_id, id );
    request->make_getProfiles( uids,  fields);
    appendRequest(request);
    return id;
}

int VKEngine::reqGetGroups()
{
    int id = m_reqCounter++;
    VKRequest *request = new VKRequest(VKRequest::getGroups, mid, sid, secret, app_id, id );
    request->make_getGroups();
    appendRequest(request);
    return id;
}

//друзья

//статусы
int VKEngine::reqActivity_get(QString uid)
{
    int id = m_reqCounter++;
    VKRequest *request = new VKRequest(VKRequest::Activity_get, mid, sid, secret, app_id, id );
    request->make_Activity_get(uid);
    appendRequest(request);
    return id;
}
//установка статуса
int VKEngine::reqActivity_set(QString text)
{
    int id = m_reqCounter++;
    VKRequest *request = new VKRequest(VKRequest::Activity_set, mid, sid, secret, app_id, id );
    request->make_Activity_set(text);
    appendRequest(request);
    return id;
}

//друзья
int VKEngine::reqFriends_get(QStringList fields, QString name_case)
{
    int id = m_reqCounter++;
    VKRequest *request = new VKRequest(VKRequest::Friends_get, mid, sid, secret, app_id, id );
    request->make_Friends_get(fields, name_case);
    appendRequest(request);
    return id;
}

//сообщения
int VKEngine::reqMessages_getLongPollServer()
{
    int id = m_reqCounter++;
    VKRequest *request = new VKRequest(VKRequest::Messages_getLongPollServer, mid, sid, secret, app_id, id );
    request->make_Messages_getLongPollServer();
    appendRequest(request);
    return id;
}
//отправка сообщения
int VKEngine::reqMessages_send(VKMessage *message)
{
    int id = m_reqCounter++;
    VKRequest *request = new VKRequest(VKRequest::Messages_send, mid, sid, secret, app_id, id );
    request->make_Messages_send(message);
    appendRequest(request);
    return id;
}
//пометка сообщения как прочитанного
int VKEngine::reqMessages_markAsRead(QStringList mids)
{
    int id = m_reqCounter++;
    VKRequest *request = new VKRequest(VKRequest::Messages_markAsRead, mid, sid, secret, app_id, id );
    request->make_Messages_markAsRead(mids);
    appendRequest(request);
    return id;
}
//подгрузка собщений
int VKEngine::reqMessages_get(QString offset, QString count, QString out,
                              QString filters, QString preview_length,
                              QString time_offset)
{
    int id = m_reqCounter++;
    VKRequest *request = new VKRequest(VKRequest::Messages_get, mid, sid, secret, app_id, id );
    request->make_Messages_get(out, offset, count, filters, preview_length, time_offset);
    appendRequest(request);
    return id;
}

int VKEngine::reqMessages_getHistory(QString uid, QString offset, QString count)
{
    int id = m_reqCounter++;
    VKRequest *request = new VKRequest(VKRequest::Messages_getHistory, mid, sid, secret, app_id, id );
    request->make_Messages_getHistory(uid, offset, count);
    //qDebug() << request->requestUrlPath;
    appendRequest(request);
    return id;
}

int VKEngine::reqAudio_get(QString uid, QString gid, QString aids,
                           QString need_user)
{
    int id = m_reqCounter++;
    VKRequest *request = new VKRequest(VKRequest::Audio_get, mid, sid, secret, app_id, id );
    request->make_Audio_get(uid, gid, aids, need_user);
    appendRequest(request);
    return id;
}

int VKEngine::reqAudio_getById(QStringList audios)
{
    int id = m_reqCounter++;
    VKRequest *request = new VKRequest(VKRequest::Audio_getById, mid, sid, secret, app_id, id );
    request->make_Audio_getById(audios);
    appendRequest(request);
    return id;
}

int VKEngine::reqPhotos_getAlbums(QString uid, QStringList aids)
{
    int id = m_reqCounter++;
    VKRequest *request = new VKRequest(VKRequest::Photos_getAlbums, mid, sid, secret, app_id, id );
    request->make_Photos_getAlbums(uid, aids);
    appendRequest(request);
    return id;
}

int VKEngine::reqPhotos_getById(QStringList photos)
{
    int id = m_reqCounter++;
    VKRequest *request = new VKRequest(VKRequest::Photos_getById, mid, sid, secret, app_id, id );
    request->make_Photos_getById(photos);
    appendRequest(request);
    return id;
}

int VKEngine::reqVideo_get(QString uid, QStringList videos,
                           QString width, QString count,
                           QString offset)
{
    int id = m_reqCounter++;
    VKRequest *request = new VKRequest(VKRequest::Video_get, mid, sid, secret, app_id, id );
    request->make_Video_get(uid, videos, width, count, offset);
    appendRequest(request);
    return id;
}

int VKEngine::reqVideoUrl(QString videoInfo)
{
    httpReqType = Video_getUrl;

    QString videoUrl = "http://vkontakte.ru/video"+videoInfo;

    debugView->load(QUrl(videoUrl));

}

//отсутствует в апи
void VKEngine::videoUrlReceived()
{
    if ( httpReqType != Video_getUrl ){
        return;
    }

    QStringList flashVars = debugView->page()->mainFrame()->documentElement().findFirst("[name=flashvars]").attribute("value").split("&");

    //qDebug () << flashVars;

    bool isBroken = false;

    QString url = "http://";
    foreach(QString param, flashVars){
        if (param.contains("host")){
            url += param.split("=").at(1) + "/assets/videos/";
        } else if (param.contains("vtag")){
            if (param.split("=").at(1).length() < 12){
                url += param.split("=").at(1) + "-";
                isBroken = true;
            } else {
                url += param.split("=").at(1);
            }

        } else if (param.contains("vkid")){
            url += param.split("=").at(1) + ".vk.flv";
            break;
        }
    }
    //qDebug() << QUrl(url).toString() << debugView->url().toString();
    emit videoUrlExtracted(QUrl(url), isBroken);
}

int VKEngine::reqAudio_add(QString aid, QString oid, QString gid)
{
    int id = m_reqCounter++;
    VKRequest *request = new VKRequest(VKRequest::Audio_add, mid, sid, secret, app_id, id );
    request->make_Audio_add(aid, oid, gid);
    appendRequest(request);
    return id;
}

int VKEngine::reqMessages_delete(QString mid)
{
    int id = m_reqCounter++;
    VKRequest *request = new VKRequest(VKRequest::Messages_delete, mid, sid, secret, app_id, id );
    request->make_Messages_delete(mid);
    qDebug() << request->requestUrlPath;
    appendRequest(request);
    return id;
}
