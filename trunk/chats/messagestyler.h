#ifndef MESSAGESTYLER_H
#define MESSAGESTYLER_H

#include <QObject>
#include <QMap>
#include <QUrl>

class VKMessage;
class QHttp;
class VKRequest;
class VKEngine;
class Emoticons;

//заботится о смайликах, а также о представлении аудио и видео,
//которые будут приходить от пользователей.
//объект данного класса будет обрабатывать все входящие сообщения

class MessageStyler : public QObject
{
    Q_OBJECT
public:
    explicit MessageStyler(VKEngine *parent = 0);

    Emoticons *emoticons;

signals:
    void stylingComplete(VKMessage *mess);
    void audioFinded();

public slots:
    void makeStyle(VKMessage *mess);
    void makeFastStyle(VKMessage *mess);


private:
    void styleEmails(VKMessage *mess);
    void styleLinks(VKMessage *mess);
    void styleSmiles(VKMessage *mess);
    void styleImages(VKMessage *mess);
    void styleAudio(VKMessage *mess);
    void styleVideo(VKMessage *mess);
    void encodeEscaped(VKMessage *mess);

    VKEngine *vkEngine;

    //audio
    int getAudioById, getPhotoById, getVideoUrlId;
    VKMessage *curMess;

private slots:
    void audioGetByIdFinished(VKRequest *req);
    void photoGetByIdFinished(VKRequest *req);
    void videoGetUrlFinished(QUrl url, bool isBroken);

};

#endif // MESSAGESTYLER_H
