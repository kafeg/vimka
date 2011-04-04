#include "vkroster.h"
#include "../vimkamain.h"
#include "vkengine.h"
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include "../roster/rosterwidgetitem.h"
#include "vkrequest.h"
#include <QApplication>

#include <QDomNode>
#include <QDomNodeList>
#include <QDomElement>
#include <QListView>
#include <QSortFilterProxyModel>
#include <QDebug>
#include "../settingsmanager.h"
#include "../roster/qhttpimage.h"
#include <QFile>
#include "../roster/wwactivelabel.h"
#include "../roster/wwactivelineedit.h"
#include "../roster/rosteritemdelegate.h"
#include "../vk/vkmessage.h"
#include "../chats/chats.h"
#include "ui_vimkamain.h"
#include <QDeclarativeContext>
#include "../SlidingStackedWidget.h"

VKRoster::VKRoster(VimkaMain *rosterWindows, VKEngine *engine,
                   QListView *rosterView, QObject *parent) :
QStandardItemModel(0, 22, parent)
{
    m_rosterWindow = rosterWindows;
    m_vkEngine = engine;
    m_rosterView = rosterView;
    m_rosterView->setWordWrap (true);

    QHash<int, QByteArray> roles = roleNames();
    roles.insert(RosterProxyModel::NameRole, QByteArray("UserFullName"));
    roles.insert(RosterProxyModel::UrlRole, QByteArray("UserPhoto"));
    roles.insert(RosterProxyModel::UrlMediumRole, QByteArray("UserPhotoMedium"));
    roles.insert(RosterProxyModel::UserStatusRole, QByteArray("UserStatus"));
    roles.insert(RosterProxyModel::UserSexRole, QByteArray("UserSex"));
    roles.insert(RosterProxyModel::UserBDateRole, QByteArray("UserBDate"));
    roles.insert(RosterProxyModel::UserActivityRole, QByteArray("UserActivityXml"));
    roles.insert(RosterProxyModel::UserRateRole, QByteArray("UserRate"));
    roles.insert(RosterProxyModel::UserHomePhoneRole, QByteArray("UserPhoneHome"));
    roles.insert(RosterProxyModel::UserMobilePhoneRole, QByteArray("UserPhoneMobile"));
    roles.insert(RosterProxyModel::UserUniverstiyNameRole, QByteArray("UserUniversityName"));
    roles.insert(RosterProxyModel::UserFacultyNameRole, QByteArray("UserFacultyName"));
    roles.insert(RosterProxyModel::UserHasUnreadedMessages, QByteArray("UserHasUnreaded"));
    setRoleNames(roles);

    proxyModel = new RosterProxyModel(this, m_rosterWindow->settingsMngr);
    proxyModel->setDynamicSortFilter(true);
    proxyModel->setSourceModel(this);
    proxyModel->sort(1, Qt::AscendingOrder);

    m_rosterView->setModel(proxyModel);
    m_rosterView->setModelColumn(1);
    m_rosterView->setIconSize(QSize(32,32));

    m_onlyOnline = false;

    connect(m_vkEngine, SIGNAL(requestFinished(VKRequest *)),
            this, SLOT(receivedFriends_get(VKRequest *)));
    connect(m_vkEngine, SIGNAL(requestFinished(VKRequest *)),
            this, SLOT(receivedActivity_get(VKRequest*)));
    connect(m_vkEngine, SIGNAL(requestFinished(VKRequest *)),
            this, SLOT(receivedActivity_set(VKRequest*)));
    connect(m_vkEngine, SIGNAL(requestFinished(VKRequest *)),
            this, SLOT(receivedMessages_getUnreaded(VKRequest*)));

    connect(m_vkEngine,SIGNAL(connected()),this,SLOT(vkConnected()));
    connect(m_vkEngine,SIGNAL(disconnected()),this,SLOT(vkDisconnected()));

    connect(m_rosterWindow->activityLabel,SIGNAL(doubleClicked()),
            this,SLOT(slotStartChangeActivity()));

    connect(m_rosterWindow->activityEditor,SIGNAL(returnPressed()),
            this,SLOT(slotFinishChangeActivity()));
    connect(m_rosterWindow->activityEditor,SIGNAL(escapePressed()),
            this,SLOT(slotCancelChangeActivity()));

    //LPS Online/Offline users
    connect(m_vkEngine,SIGNAL(lps8UserOnline(QString,QString)),
            this,SLOT(slotLps8UserOnline(QString,QString)));
    connect(m_vkEngine,SIGNAL(lps9UserOfline(QString,QString)),
            this,SLOT(slotLps9UserOfline(QString,QString)));

    connect(m_rosterView,SIGNAL(doubleClicked(QModelIndex)),
            this,SLOT(slotOpenChat(QModelIndex)));
    connect(m_rosterWindow->filterEditor, SIGNAL(returnPressed()),
            this,SLOT(slotOpenFirstChat()));

    //декларативный ростер
    //qDebug() << "roles setted!" << roleNames();

    m_rosterWindow->ui->dvRoster->rootContext()->setContextProperty("rosterModel", proxyModel);

}
/*
QVariant VKRoster::data(const QModelIndex &index, int role) const
{
    switch(role){
    case NameRole:
        return item(index.row(), 3)->text();
        break;
    case UrlRole:
        return item(index.row(), 8)->text();
        break;
    default:
        return QStandardItemModel::data(index, role);
    }
}*/

void VKRoster::slotOpenChat(QModelIndex index)
{
    if (!index.isValid()){
        return;
    }

    QModelIndex srcIndex = proxyModel->mapToSource(index);
    QStandardItem *uidItem = item(srcIndex.row(), 4);
    QString uid = uidItem->text();
    QStandardItem *onlineItem = item(srcIndex.row(), 11);
    int online = onlineItem->text().toInt();

    VKMessage *mess = new VKMessage( m_vkEngine );
    mess->from_id = uid;

    QIcon status = online > 0 ? m_rosterWindow->settingsMngr->online :
                   m_rosterWindow->settingsMngr->offline;


    PersonalChat *chat = m_rosterWindow->chats->openPersonalChat(mess, status);
    m_rosterWindow->chats->showPersonalChat(chat);
    setItem(srcIndex.row(),21, new QStandardItem("0"));
#ifdef MOBILE_UI
    m_rosterWindow->slidingStacked->slideInIdx(m_rosterWindow->slidingStacked->indexOf(m_rosterWindow->chats));
#endif

}

void VKRoster::vkConnected()
{
    QStringList fields;
    fields << "first_name"
            << "last_name"
            << "sex"
            << "photo_rec"
            << "photo_medium"
            << "online"
            << "domain"
            << "bdate"
            << "lists"
            << "rate"
            << "contacts"
            << "education";
    reqFriends_getId = m_vkEngine->reqFriends_get(fields);
}

void VKRoster::vkDisconnected()
{
    clear();
}

void VKRoster::receivedActivity_get(VKRequest *req)
{
    if((req->reqType!=VKRequest::Activity_get) ||
       (reqActivity_getId != req->reqId)){
        return;
    }

    QDomDocument doc("Activity");
    doc.setContent(req->result);
    QDomElement docElem = doc.documentElement();
    QDomElement activity = docElem.elementsByTagName ( "activity" ).at(0).toElement();
    QString activityText;
    activityText = activity.text();
    activityText = activityText.replace("&quot;","\"");

    if (activityText.trimmed().isEmpty()){
        m_rosterWindow->activityLabel->setText(tr("Press for set activity"));
    }else{
        m_rosterWindow->activityLabel->setText(activityText);
    }

    m_rosterWindow->activityLabel->show();

    if (m_rosterWindow->uploadWgt->isVisible()){
        m_rosterWindow->uploadWgt->hide();
    }

    //остаётся получить список непрочитанных сообщений
    reqMessages_getUnreadedId = m_vkEngine->reqMessages_get("0","0","0","1","1","0");
}

void VKRoster::receivedActivity_set(VKRequest *req)
{
    if((req->reqType!=VKRequest::Activity_set) ||
       (reqActivity_setId != req->reqId)){
        return;
    }

    reqActivity_getId = m_vkEngine->reqActivity_get(m_vkEngine->mid);
}

void VKRoster::receivedMessages_getUnreaded(VKRequest *req)
{
    if((req->reqType!=VKRequest::Messages_get) ||
       (reqMessages_getUnreadedId != req->reqId)){
        return;
    }

    //qDebug() << req->result;

    QDomDocument doc("RosterUnreaded");
    doc.setContent(req->result);
    QDomElement docElem = doc.documentElement();
    QDomNodeList items = docElem.elementsByTagName ( "message" );
    for(int i=0; i < items.count(); i++){
        QDomElement elem = items.at(i).toElement();

        QString uid = elem.elementsByTagName("uid").at(0).toElement().text();
        //QString mid = elem.elementsByTagName("uid").at(0).toElement().text();

        QList<QStandardItem *> items = findItems(uid, Qt::MatchExactly, 4);
        if (!items.isEmpty()){
            QModelIndex index = indexFromItem(items.first());
            setItem(index.row(),21, new QStandardItem("1"));
            //qDebug() << item(index.row(),21)->text();
        }
        QApplication::processEvents();
    }
}

void VKRoster::slotStartChangeActivity()
{
    m_rosterWindow->activityLabel->hide();
    m_rosterWindow->activityEditor->setText(m_rosterWindow->activityLabel->text());
    m_rosterWindow->activityEditor->show();
    m_rosterWindow->activityEditor->setFocus();
    m_rosterWindow->activityEditor->selectAll();
}

void VKRoster::slotFinishChangeActivity()
{
    m_rosterWindow->activityEditor->hide();
    m_rosterWindow->uploadWgt->show();
    reqActivity_setId = m_vkEngine->reqActivity_set( m_rosterWindow->activityEditor->text());
}

void VKRoster::slotCancelChangeActivity()
{
    m_rosterWindow->activityEditor->hide();
    m_rosterWindow->uploadWgt->hide();
    m_rosterWindow->activityLabel->show();
}

void VKRoster::receivedFriends_get(VKRequest *req)
{
    if((req->reqType!=VKRequest::Friends_get) ||
       (reqFriends_getId != req->reqId)){
        return;
    }

    //rosterItems.clear();
    clear();

    //qDebug() << req->result;

    QDomDocument doc("Roster");
    doc.setContent(req->result);
    QDomElement docElem = doc.documentElement();
    QDomNodeList users = docElem.elementsByTagName ( "user" );
    for(int i=0; i < users.count(); i++){
        QDomElement userElem = users.at(i).toElement();

        QString uid = userElem.elementsByTagName("uid").at(0).toElement().text();
        QString first_name = userElem.elementsByTagName("first_name").at(0).toElement().text();
        QString last_name = userElem.elementsByTagName("last_name").at(0).toElement().text();
        QString sex = userElem.elementsByTagName("sex").at(0).toElement().text();
        QString photo = userElem.elementsByTagName("photo_rec").at(0).toElement().text();
        QString photo_medium = userElem.elementsByTagName("photo_medium").at(0).toElement().text();
        QString domain = userElem.elementsByTagName("domain").at(0).toElement().text();
        QString bdate = userElem.elementsByTagName("bdate").at(0).toElement().text();
        QString online = userElem.elementsByTagName("online").at(0).toElement().text();
        QString rate = userElem.elementsByTagName("rate").at(0).toElement().text();
        QString home_phone = userElem.elementsByTagName("home_phone").at(0).toElement().text();
        QString mobile_phone = userElem.elementsByTagName("mobile_phone").at(0).toElement().text();
        QString university_name = userElem.elementsByTagName("university_name").at(0).toElement().text();
        QString faculty_name = userElem.elementsByTagName("faculty_name").at(0).toElement().text();

        QString onlineText = online.toInt() ? tr("Online") : tr("Offline");
        if (bdate.isEmpty()){
            bdate = tr("undefined");
        }
        //qDebug() << home_phone << mobile_phone << university_name << faculty_name;
        //qDebug() << userElem
        QString offline = online.toInt() ? "0" : "1";
        //то что показывается..
        //RosterWidgetItem *rosterItem = new RosterWidgetItem(uid);
        bool offNotShowStatus = true;
        if (offNotShowStatus && offline.toInt()){
            onlineText = "";
        }
        RosterWidgetItem *iconItem = new RosterWidgetItem(first_name+" "+last_name + "\n" + onlineText);

        QString sexText;// = tr("Sex: ");
        switch(sex.toInt()){
        case 0: sexText += tr("undefined sex");
            break;
        case 1: sexText += tr("female");
            break;
        case 2: sexText += tr("male");
            break;
        }

        //RosterWidgetItem *iconItem = new RosterWidgetItem(first_name+" "+last_name + " " + onlineText);

        QHttpImage *httpImage = new QHttpImage(this);
        httpImage->setUrl(photo);
        connect(httpImage, SIGNAL(loadedIcon(QIcon)),
                iconItem,SLOT(applyIcon(QIcon)));
        httpImage->load();

        //для сортировки
        QStandardItem *username = new QStandardItem(offline+first_name+last_name);
        //для фильтрации
        QStandardItem *status = new QStandardItem(online);
        //для поиска
        QStandardItem *fullname = new QStandardItem(first_name+" "+last_name);

        //url активности
        VKRequest *activityReq = m_vkEngine->getEmptyReq(VKRequest::Activity_get);
        activityReq->make_Activity_get(uid);
        QString actUrl = m_vkEngine->api_url + "?" + activityReq->requestUrlPath;
        //qDebug() << actUrl;

        appendRow(QList<QStandardItem *>() << iconItem
                  << username << status << fullname
                  << new QStandardItem(uid) << new QStandardItem(first_name)
                  << new QStandardItem(last_name) << new QStandardItem(sex)
                  << new QStandardItem(photo) << new QStandardItem(domain)
                  << new QStandardItem(bdate) << new QStandardItem(online)
                  << new QStandardItem(onlineText) << new QStandardItem(sexText)
                  << new QStandardItem(actUrl) << new QStandardItem(rate)
                  << new QStandardItem(photo_medium) << new QStandardItem(home_phone)
                  << new QStandardItem(mobile_phone) << new QStandardItem(university_name)
                  << new QStandardItem(faculty_name) << new QStandardItem("0") );

        //QApplication::processEvents();

        /*
<user>
  <uid>1249682</uid>
  <first_name>Анечка</first_name>
  <last_name>Василенко</last_name>
  <sex>1</sex>
  <photo>http://cs444.vkontakte.ru/u1249682/c_b82dca4a.jpg</photo>
  <domain>ladychaplin</domain>
  <bdate>7.4.1990</bdate>
  <online>0</online>
 </user>
*/
    }

    reqActivity_getId = m_vkEngine->reqActivity_get(m_vkEngine->mid);
    if(m_rosterWindow->ui->dvRoster->source().isEmpty()){
        m_rosterWindow->ui->dvRoster->setSource(QUrl("qrc:/declarative/roster/roster.qml"));
    }
}

void VKRoster::setFilter(QString filter)
{
    QRegExp regExp(filter, Qt::CaseInsensitive);
    proxyModel->setFilterRegExp(regExp);

    proxyModel->sort(1,Qt::AscendingOrder);
}

void VKRoster::setShowOnlyOnline(bool online)
{
    m_onlyOnline = online;
    proxyModel->setFilterKeyColumn(2);
    if (m_onlyOnline){
        setFilter("1");
    }else{
        setFilter("");
    }
    proxyModel->sort(1,Qt::AscendingOrder);
}

void VKRoster::findUser(QString username)
{
    proxyModel->setFilterKeyColumn(3);
    if (!username.isEmpty()){
        setFilter(username);
    }else{
        setShowOnlyOnline(m_onlyOnline);
    }
}

void VKRoster::slotLps8UserOnline(QString uid, QString flags)
{
    //qDebug() << "Online";
    QModelIndex index = findItems(uid,Qt::MatchExactly, 4).at(0)->index();
    QStandardItem *statusItem = item(index.row(), 2);
    QStandardItem *usernameItem = item(index.row(), 1);
    QStandardItem *iconItem = item(index.row(), 0);
    QString first_name = item(index.row(), 5)->text();
    QString last_name = item(index.row(), 6)->text();
    QStandardItem *onlineTextItem = item(index.row(), 12);

    iconItem->setText( first_name+" "+last_name + "\n" +  tr("Online") );
    usernameItem->setText("0"+first_name+last_name);
    statusItem->setText("1");
    onlineTextItem->setText(tr("Online"));
    reset();

    proxyModel->sort(1,Qt::AscendingOrder);
}

void VKRoster::slotLps9UserOfline(QString uid, QString flags)
{
    //qDebug() << "Offline";
    QModelIndex index = findItems(uid,Qt::MatchExactly, 4).at(0)->index();
    QStandardItem *statusItem = item(index.row(), 2);
    QStandardItem *usernameItem = item(index.row(), 1);
    QStandardItem *iconItem = item(index.row(), 0);
    QStandardItem *onlineTextItem = item(index.row(), 12);
    QString first_name = item(index.row(), 5)->text();
    QString last_name = item(index.row(), 6)->text();

    //iconItem->setText( first_name+" "+last_name + "\n" +  tr("Offline") );
    iconItem->setText( first_name+" "+last_name + "\n" );
    usernameItem->setText("1"+first_name+last_name);
    statusItem->setText("0");
    onlineTextItem->setText("");
    reset();

    proxyModel->sort(1,Qt::AscendingOrder);
}

void VKRoster::slotOpenFirstChat()
{
    QModelIndex firstIndex = proxyModel->index(0,0);
    if (firstIndex.isValid()){
        slotOpenChat(firstIndex);
    }
}
