#ifndef VKMESSAGE_H
#define VKMESSAGE_H

#include <QObject>

class VKEngine;

class VKMessage : public QObject
{
    Q_OBJECT

    friend class MessageStyler;
public:
    /*
    +1	UNREAD	сообщение непрочитано
    +2	OUTBOX	исходящее сообщение
    +4	REPLIED	на сообщение был создан ответ
    +8	IMPORTANT	помеченное сообщение
    +16	CHAT	сообщение отправлено через чат
    +32	FRIENDS	сообщение отправлено другом
    +64	SPAM	сообщение помечено как "Спам"
    +128	DELЕTЕD	сообщение удалено (в корзине)
    +256	FIXED	сообщение проверено пользователем на спам
    +512	MEDIA	сообщение содержит медиаконтент
    */
    //Флаги сообщения
    int flags;

    explicit VKMessage(VKEngine *engine);

    QString local_id, from_id, to_id, message_id, subject, date, styledText, local_from_id;
    bool isFirst;

    //{ ts: 196851367, updates: [ [ 4, 16929, 1, 85635407, 1280307577, ' ... ', 'hello' ] ] }
    //4,$local_id,$flags,$from_id,$message_id,$subject,$text -- добавление нового сообщения

    void setFlags(int flags);
    void setText(QString text){ this->text = text; this->styledText = text; }
    QString getText(){ return text; }
    bool isDeleted() { return m_deleted; }
    bool fromHistory;
signals:
    void changed(VKMessage *mess);

public slots:

private slots:
    void fromlps0DeleteMessage(QString local_id);
    void fromlps1ReplaceMessageFlags(QString local_id, int flags);
    void fromlps2SetMessageFlags(QString local_id, int mask);
    void fromlps3ResetMessageFlags(QString local_id, int mask);


private:
    bool m_deleted;
    VKEngine *vkEngine;
    QString text;

};

#endif // VKMESSAGE_H
