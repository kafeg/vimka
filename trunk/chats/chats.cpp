#include "chats.h"
#include "ui_chats.h"
#include "../vimkamain.h"
#include "../trayicon.h"
#include "../vk/vkmessage.h"
#include "../vk/vkengine.h"
#include "personalchat.h"
#include <QTimer>
#include "chattextedit.h"
#include "ui_vimkamain.h"
#include <QTabWidget>
#ifdef QT_PHONON_LIB
#include "mediaplayer.h"
#endif
#include "declarative/albumsmodel.h"
#include <QDesktopWidget>
#include "../settingsmanager.h"
#include "../vk/vkroster.h"
#include "../SlidingStackedWidget.h"

Chats::Chats(VimkaMain *rosterWindow, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Chats)
{
    ui->setupUi(this);
    m_rosterWindow = rosterWindow;
    setWindowIcon(m_rosterWindow->settingsMngr->online);
    twChats = ui->twChats;
    vkEngine = m_rosterWindow->vkEngine;
    connect(vkEngine,SIGNAL(lps4MessageReceived(VKMessage*)),
            this,SLOT(messageReceived(VKMessage*)));

    hide();

    connect(m_rosterWindow->trayIcon,SIGNAL(onIncMessageClick()),
            this,SLOT(showRecMessages()));
    connect(m_rosterWindow->trayIcon->trayIcon,SIGNAL(messageClicked()),
            this,SLOT(showRecMessages()));
    connect(m_rosterWindow->trayIcon,SIGNAL(beforeQuit()),
            this,SLOT(stopPlayer()));

    connect(twChats,SIGNAL(currentChanged ( int  )),this,SLOT(currentChatChanged(int)));
#ifdef QT_PHONON_LIB
    pagePlayer = new MediaPlayer (this, m_rosterWindow->settingsMngr);
    //pagePlayer->m_MediaNotificationObject.setCurrentSource(Phonon::MediaSource(m_rosterWindow->settingsMngr->styleDir()+"/sounds/incomming.mp3"));
#endif
    QRect rect = QApplication::desktop()->geometry();
    this->move (rect.center() - this->rect().center());

    //m_MediaObject.play();

}

void Chats::stopPlayer()
{
#ifdef QT_PHONON_LIB
    pagePlayer->m_MediaObject.stop();
#endif
}

void Chats::hideEvent(QHideEvent *e)
{
    stopPlayer();
    QWidget::hideEvent(e);
}

Chats::~Chats()
{
    stopPlayer();
    delete ui;
}

void Chats::messageReceived(VKMessage *incommingMessage)
{
    PersonalChat *chat = openPersonalChat(incommingMessage);

    show();
    if (!(incommingMessage->flags & 2) && ((tabIdFromUid(chat->fromId) != twChats->currentIndex())
                                           || windowState() != Qt::WindowActive)) {
        m_rosterWindow->trayIcon->startIncMessage();
        chat->startMigalko();
        if (m_rosterWindow->ui->tbSound->isChecked()){
#ifdef QT_PHONON_LIB
            pagePlayer->incomming();
#endif
        }
    }

    chat->messageReceived(incommingMessage);

}

void Chats::clearList()
{
    opennedChats.clear();
    //showedChatsPageIndex.clear();
}

PersonalChat *Chats::openPersonalChat(VKMessage *message, QIcon statusIcon)
{
    if (statusIcon.isNull()){
        statusIcon = m_rosterWindow->settingsMngr->online;
    }

    QString from_id = message->from_id;

    PersonalChat *chat;
    if (opennedChats.contains(from_id)){
        chat = opennedChats.value(from_id);
        if (!chat->showed){
            twChats->addTab(chat,"");
            chat->showChat();
        }
    }else{
#ifdef QT_PHONON_LIB
        chat = new PersonalChat(from_id, this, vkEngine, pagePlayer, albumsModel);
#else
        chat = new PersonalChat(from_id, this, vkEngine, albumsModel);
#endif
        opennedChats.insert(from_id, chat);

        connect(chat->chatEdit, SIGNAL(focusInChatEdit()),
                this, SLOT(focusChanged()));
        //connect(twChats,SIGNAL(currentChanged ( int  )),
        //        chat,SLOT(currentChatChanged(int)));

        //connect(chat, SIGNAL(profileLoaded(QString)), this, SLOT(chatProfileLoaded(QString)));

        twChats->addTab(chat,tr("Loading..."));
        //управление мать его оповещениями...
        //говнокод знаю. но это всё логика интерфейса,
        //которую я заманался делать три дня... так что пусть будет хоть так
        //ведёт себя прилично. честно =)
        //showedChatsPageIndex.insert(from_id, chat->tabId);
        //showedChatsIndexPage.insert(chat->tabId, from_id);

        chat->showChat();
    }

    chatSetTabIcon(from_id, statusIcon);

    return chat;
}

void Chats::showPersonalChat(PersonalChat *chat)
{
    if (isMinimized()){
        showNormal();
    }

    twChats->setCurrentWidget(chat);
    chat->showChat();
    show();
    activateWindow();
    chat->chatEdit->setFocus();
}

void Chats::showRecMessages()
{
    //bool hasNerMess = false;
    /*for (int i=0; i <twChats->count();i++){
        PersonalChat *chat = static_cast<PersonalChat *>(twChats->widget(i));
        if (chat->hasNewMessages){
            twChats->setCurrentIndex(i);
            break;
        }
        QApplication::processEvents();
    }*/
    m_rosterWindow->trayIcon->stopIncMessage();
    //if (hasNerMess){
    show();
    showNormal();
    activateWindow();
    window()->activateWindow();
    window()->showNormal();
    //}
}

void Chats::openInfoPage(QString pageName, QWidget *page)
{
    if (!opennedInfoPages.contains(pageName)){
        twChats->addTab(page,pageName);
        opennedInfoPages.insert(pageName,page);
    }
    twChats->setCurrentWidget(page);
}

void Chats::showInfoPage(QString pageName)
{
    if (opennedInfoPages.contains(pageName)){
        twChats->setCurrentWidget(opennedInfoPages.value(pageName));
        opennedInfoPages.value(pageName)->show();
        show();
        activateWindow();
    }
}

void Chats::hideInfoPage(QString pageName)
{
    if (opennedInfoPages.contains(pageName)){
        twChats->setCurrentWidget(opennedInfoPages.value(pageName));
        if (twChats->count() == 1){
            twChats->currentWidget()->hide();
            twChats->removeTab(0);
            hide();
        }else{
            twChats->removeTab(twChats->currentIndex());
            twChats->currentWidget()->hide();
        }
    }
}

void Chats::on_twChats_tabCloseRequested(int index)
{
    //ui->twChats->widget(index)->

    QMapIterator<QString, PersonalChat *> i(opennedChats);
    while (i.hasNext()){
        i.next();

        if (tabIdFromUid(i.key()) == index){
            i.value()->hideChat();
            break;
        }
    }

    twChats->removeTab(index);

    if (twChats->count()==0){
        hide();
#ifdef QT_PHONON_LIB
        pagePlayer->m_MediaObject.stop();
#endif
#ifdef MOBILE_UI
        m_rosterWindow->slidingStacked->slideInIdx(m_rosterWindow->slidingStacked->indexOf(m_rosterWindow->ui->rosterPage));
#endif
    }
}

void Chats::currentChatChanged(int index)
{
    if (twChats->count() == 0){
        m_rosterWindow->trayIcon->stopIncMessage();
        return;
    }

    QMapIterator<QString, PersonalChat *> i(opennedChats);
    while (i.hasNext()){
        i.next();
        if (tabIdFromUid(i.value()->fromId) == index){
            i.value()->stopMigalko();
            //setWindowTitle(i.value()->fromFullName);
            break;
        }
    }

    bool hasNerMess = false;
    for (int i=0; i <twChats->count();i++){
        PersonalChat *chat = static_cast<PersonalChat *>(twChats->widget(i));

        if (chat->hasNewMessages){
            hasNerMess = true;
            break;
        }
        QApplication::processEvents();
    }

    //if (!hasNerMess){
    m_rosterWindow->trayIcon->stopIncMessage();
    //}

}

void Chats::focusChanged()
{
    currentChatChanged(0);
}

void Chats::showEvent(QShowEvent *e)
{
    currentChatChanged(0);
    QWidget::showEvent(e);
}

void Chats::vkConnected()
{
    //модель достающая фотки и альбомы текущего пользователя
    albumsModel = new AlbumsModel(vkEngine);
    albumsModel->select();
}

int Chats::tabIdFromUid(QString uid)
{
    int id = -1;
    for(int i = 0; i <twChats->count(); i++ ){
        if (twChats->widget(i)->objectName()==uid){
            id = i;
            break;
        }
    }
    return id;
}

void Chats::chatSetTabIcon(QString fromId, QIcon icon)
{
    twChats->setTabIcon(tabIdFromUid(fromId), icon);
}

void Chats::chatSetTabText(QString fromId, QString name)
{
    twChats->setTabText(tabIdFromUid(fromId), name);
}
