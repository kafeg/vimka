#ifndef VKREQUEST_H
#define VKREQUEST_H

#include <QObject>
#include <QStringList>

class VKMessage;

class VKRequest
{
public:

    enum Request{
        //пользователи
        getProfiles,
        getGroups,
        //друзья
        Friends_get,
        //статусы
        Activity_get,
        Activity_set,
        //фотки и альбомы
        Photos_getAlbums,
        Photos_get,
        Photos_getById,

        //аудио
        Audio_get,
        Audio_getById,
        Audio_add,

        //видео
        Video_get,

        //сообщения
        Messages_get,
        Messages_send,
        Messages_delete,
        Messages_markAsRead,// – помечает сообщения, как прочитанные.
        Messages_getHistory,
        Messages_getLongPollServer

    };

    explicit VKRequest(Request reqType, QString mid, QString sid,
                       QString secret, QString api_id, int _reqId);

    QString requestTypeText;
    QString requestUrlPath;
    int reqId;
    Request reqType;
    QString result;

    void make_getProfiles(QStringList uids, QStringList fields);
    void make_getGroups();
    void make_Friends_get(QStringList fields, QString name_case);
    void make_Activity_get(QString uid);
    void make_Activity_set(QString text);
    void make_Photos_getAlbums(QString uid, QStringList aids);
    void make_Photos_get(QString uid, QString aid, QStringList pids = QStringList());
    void make_Photos_getById(QStringList photos);
    void make_Audio_get(QString uid, QString gid = "-1", QString aids = "-1",
                        QString need_user = "0");
    void make_Audio_getById(QStringList audios);
    void make_Audio_add(QString aid, QString oid, QString gid = "-1");
    void make_Video_get(QString uid, QStringList videos = QStringList(),
                        QString width = "160", QString count = "-1",
                        QString offset = "0");
    void make_Messages_get(QString offset, QString count = "10", QString out = "0",
                           QString filters = "0", QString preview_length = "0",
                           QString time_offset = "0");
    void make_Messages_send(VKMessage *message);
    void make_Messages_markAsRead(QStringList mids);
    void make_Messages_getHistory(QString uid, QString offset, QString count = "10");
    void make_Messages_getLongPollServer();
    void make_Messages_delete(QString mid);

signals:

public slots:

private:

    QStringList m_requestSrc, m_sigSrc;
    QString m_mid, m_sid, m_secret, m_api_id, m_sig;

    void makeSig();

};

#endif // VKREQUEST_H
