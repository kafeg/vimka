#include "trayicon.h"
#include "vimkamain.h"
#include <QAction>
#include <QApplication>
#include <QMenu>
#include <QTimer>
#include "settingsmanager.h"
#include "vk/vkengine.h"
#include "vk/vkmessage.h"

TrayIcon::TrayIcon(VimkaMain *rosterWindow) :
        QObject(rosterWindow)
{
    m_rosterWindow = rosterWindow;
    createActions();
    createTrayIcon();

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    setIcon(Offline);
    trayIcon->show();

    incommingMessageTray = new QTimer();
    incommingMessageTray->setInterval(500);
    connect(incommingMessageTray, SIGNAL(timeout()),
            this,SLOT(onIncTimer()));

    vkEngine = m_rosterWindow->vkEngine;
    connect(vkEngine,SIGNAL(lps4MessageReceived(VKMessage*)),
            this,SLOT(messageReceived(VKMessage*)));
}

void TrayIcon::messageReceived(VKMessage *mess)
{
    if (!(mess->flags & 2)) {
        trayIcon->showMessage(tr("New message!"), tr("Click for see new messages..."), QSystemTrayIcon::Information, 5000);
    }
}

void TrayIcon::createActions()
{
    minimizeAction = new QAction(m_rosterWindow->settingsMngr->minimize,tr("Mi&nimize"), this);
    connect(minimizeAction, SIGNAL(triggered()), m_rosterWindow, SLOT(hide()));

    maximizeAction = new QAction(m_rosterWindow->settingsMngr->maximize,tr("Ma&ximize"), this);
    connect(maximizeAction, SIGNAL(triggered()), m_rosterWindow, SLOT(showMaximized()));

    restoreAction = new QAction(m_rosterWindow->settingsMngr->restore,tr("&Restore"), this);
    connect(restoreAction, SIGNAL(triggered()), m_rosterWindow, SLOT(showNormal()));

    quitAction = new QAction(m_rosterWindow->settingsMngr->quit,tr("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), this, SLOT(quit()));
}

void TrayIcon::quit()
{
    emit beforeQuit();
    //sleep(0.3);
    qApp->quit();
}

void TrayIcon::createTrayIcon()
{
    trayIconMenu = new QMenu(m_rosterWindow);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}

void TrayIcon::setIcon(StateIcon index)
{
    QIcon icon;
    iconIndex = index;
    bool onoffmess = false;
    switch(index){
    case Online:
        icon = m_rosterWindow->settingsMngr->online;
        onoffmess = false;
        break;
    case OnlineRecMess:
        icon = m_rosterWindow->settingsMngr->incomming;
        onoffmess = true;
        break;
    case Offline:
        icon = m_rosterWindow->settingsMngr->offline;
        onoffmess = false;
        break;

    case Connecting:
        icon = m_rosterWindow->settingsMngr->connecting;
        onoffmess = false;
        break;
    }

    trayIcon->setIcon(icon);
    if (!onoffmess){
        m_rosterWindow->setWindowIcon(icon);
    }
}

void TrayIcon::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        if (!incommingMessageTray->isActive()){
            if(m_rosterWindow->isVisible()){
                m_rosterWindow->hide();
            }else{
                m_rosterWindow->showNormal();
            }
        }else{
            emit onIncMessageClick();
        }
        break;
     case QSystemTrayIcon::MiddleClick:
        //showMessage();
        break;
     default:
        ;
    }
}

void TrayIcon::startIncMessage()
{
    incommingMessageTray->start();
}

void TrayIcon::stopIncMessage()
{
    incommingMessageTray->stop();
    setIcon(Online);
}

void TrayIcon::onIncTimer()
{
    if (iconIndex == Online){
        setIcon(OnlineRecMess);
    }else{
        setIcon(Online);
    }
}
