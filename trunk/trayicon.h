#ifndef TRAYICON_H
#define TRAYICON_H

#include <QObject>
#include <QSystemTrayIcon>

class QAction;
class VimkaMain;
class QMenu;
class VKEngine;
class VKMessage;

class TrayIcon : public QObject
{
    Q_OBJECT
    friend class VimkaMain;
public:
    explicit TrayIcon(VimkaMain *rosterWindow );

    enum StateIcon {
        Online,
        OnlineRecMess,
        Offline,
        Connecting
    };

    QSystemTrayIcon *trayIcon;

    void startIncMessage();
    void stopIncMessage();

signals:
    void onIncMessageClick();
    void beforeQuit();

public slots:
    void setIcon(StateIcon index);

private:
    void createActions();
    void createTrayIcon();

    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;

    VimkaMain *m_rosterWindow;

    QMenu *trayIconMenu;
    QTimer *incommingMessageTray;

    StateIcon iconIndex;
    VKEngine *vkEngine;

private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void onIncTimer();
    void quit();

    void messageReceived(VKMessage *mess);

};

#endif // TRAYICON_H
