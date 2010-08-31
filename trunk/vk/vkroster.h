#ifndef VKROSTER_H
#define VKROSTER_H

#include <QObject>
#include <QMap>
#include <QStandardItemModel>
#include "../roster/rosterproxymodel.h"

class VimkaMain;
class VKEngine;
class VKRequest;
class QTreeWidget;
class QListView;
//class QHttpImage;

class VKRoster : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit VKRoster(VimkaMain *rosterWindows,  VKEngine *engine,
                      QListView *rosterView, QObject *parent = 0);

    //Variant data(const QModelIndex &index, int role) const;

signals:

public slots:
    void setShowOnlyOnline(bool online);
    void findUser(QString username);

    //Chats
    void slotOpenChat(QModelIndex index);
    void slotOpenFirstChat();

private slots:

    void vkConnected();
    void vkDisconnected();

    void receivedFriends_get(VKRequest *req);
    void receivedActivity_get(VKRequest *req);
    void receivedActivity_set(VKRequest *req);
    void receivedMessages_getUnreaded(VKRequest *req);

    void slotStartChangeActivity();
    void slotFinishChangeActivity();
    void slotCancelChangeActivity();

    //LPS Online/Offline
    void slotLps8UserOnline(QString uid, QString flags);
    void slotLps9UserOfline(QString uid, QString flags);

private:
    void setFilter(QString filter);
    VimkaMain *m_rosterWindow;
    VKEngine *m_vkEngine;
    QListView *m_rosterView;
    RosterProxyModel *proxyModel;
    bool m_onlyOnline;

    int reqFriends_getId, reqActivity_getId, reqActivity_setId,
    reqMessages_getUnreadedId;

};

#endif // VKROSTER_H
