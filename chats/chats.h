#ifndef CHATS_H
#define CHATS_H

#include <QWidget>
#include <QMap>
#include <QIcon>

namespace Ui {
    class Chats;
}

class VimkaMain;
class VKMessage;
class VKEngine;
class PersonalChat;
class QTabWidget;
class MediaPlayer;
class AlbumsModel;

class Chats : public QWidget
{
    Q_OBJECT

public:
    explicit Chats( VimkaMain *rosterWindow, QWidget *parent = 0);
    ~Chats();

    void openInfoPage(QString pageName, QWidget *page);
    void showInfoPage(QString pageName);
    void hideInfoPage(QString pageName);

    PersonalChat *openPersonalChat(VKMessage *message, QIcon statusIcon = QIcon());
    void showPersonalChat(PersonalChat *chat);
    void showEvent(QShowEvent *e);

    //фотки юзера
    AlbumsModel *albumsModel;

    void hideEvent(QHideEvent *e);

    VimkaMain *m_rosterWindow;

    void clearList();
    void chatSetTabIcon(QString fromId, QIcon icon);
    void chatSetTabText(QString fromId, QString name);

private:
    Ui::Chats *ui;
    VKEngine *vkEngine;

    QMap<QString, PersonalChat*> opennedChats;
    QMap<QString, QWidget*> opennedInfoPages;

    QTabWidget *twChats;

    MediaPlayer *pagePlayer;

    int tabIdFromUid(QString uid);

public slots:

    void messageReceived(VKMessage *incommingMessage);
    void showRecMessages();
    void vkConnected();

private slots:

    void on_twChats_tabCloseRequested(int index);
    void currentChatChanged(int index);
    void focusChanged();
    void stopPlayer();
};

#endif // CHATS_H
