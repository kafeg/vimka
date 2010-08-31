#include "messagestyler.h"
#include "../vk/vkmessage.h"
#include <QDebug>
#include <QHttp>
#include "../vk/vkrequest.h"
#include <QDomNode>
#include <QDomNodeList>
#include <QDomElement>
#include "../vk/vkengine.h"
#include "emoticons.h"

MessageStyler::MessageStyler(VKEngine *parent) :
        QObject(parent)
{
    //m_http = new QHttp("vkontakte.ru", 80, this);
    vkEngine = parent;

    emoticons = new Emoticons(parent);

    connect(vkEngine, SIGNAL(requestFinished(VKRequest *)),
            this, SLOT(audioGetByIdFinished(VKRequest *)));
    connect(vkEngine, SIGNAL(requestFinished(VKRequest *)),
            this, SLOT(photoGetByIdFinished(VKRequest *)));
    connect(vkEngine,SIGNAL(videoUrlExtracted(QUrl, bool)),
            this,SLOT(videoGetUrlFinished(QUrl,bool)));
}

//всё просто - в !определённом! порядке вызываем функции обработки стиля сообщения.
//эта функция обрабатывает пришедшие сообщения
void MessageStyler::makeStyle(VKMessage *mess)
{
    encodeEscaped(mess);
    styleLinks(curMess);
    styleSmiles(mess);
    styleImages(mess);
}
//для сообщений из истории, там нет необходимости обрабатывать аудио,
//рисунки и веидео - это будет толкьо лишней нагрузкой
void MessageStyler::makeFastStyle(VKMessage *mess)
{
    encodeEscaped(mess);

    styleLinks(mess);

    styleSmiles(mess);
}

void MessageStyler::styleSmiles(VKMessage *mess)
{
    mess->styledText = emoticons->replaceEmoticons(mess->styledText);
}

void MessageStyler::encodeEscaped(VKMessage *mess)
{
    curMess = mess;

    mess->styledText = mess->text.replace("\\/", "/");
    //
    mess->styledText = mess->styledText.replace("&lt;*&gt;", "<*>");
    mess->styledText = mess->styledText.replace("&lt;", "<");
    mess->styledText = mess->styledText.replace("&gt;", ">");
    mess->styledText = mess->styledText.replace("\\\\", "\\");
    mess->styledText = mess->styledText.replace("&quot;", "\"");
    mess->styledText = mess->styledText.replace("kf_plus", "+");
}

void MessageStyler::styleImages(VKMessage *mess)
{
    //бла<*>[[photo13577027_140238600]]

    if (!mess->styledText.contains("<*>[[photo")){
        styleAudio(mess);
        return;
    }

    QString tags = mess->styledText.left(mess->styledText.indexOf("<*>"));

    QString photoInfo = mess->styledText.right(mess->styledText.count() -
                                               (mess->styledText.lastIndexOf("<*>")+10));

    mess->styledText = tags + "<br/>";

    photoInfo = photoInfo.remove("]]");

    getPhotoById = vkEngine->reqPhotos_getById(QStringList() << photoInfo);

}

void MessageStyler::styleAudio(VKMessage *mess)
{
    //todo разбор по флагам когда будет такой метод в апи.

    //blablabla<*>[[audio35418232_80782516]]
    if (!mess->styledText.contains("<*>[[audio")){
        styleVideo(mess);
        return;
    }

    QString tags = mess->styledText.left(mess->styledText.indexOf("<*>"));

    QString audioInfo = mess->styledText.right(mess->styledText.count() -
                                               (mess->styledText.lastIndexOf("<*>")+10));

    mess->styledText = tags + " ";

    audioInfo = audioInfo.remove("]]");

    if (!( mess->flags & 2)){
        QString addUrl = "<a id=\"id"+audioInfo.split("_").at(1)+"\" href=\"Audio_add_"+audioInfo+"\">"+tr("Add to my page")+"</a></br>";

        mess->styledText += addUrl;
    }

    getAudioById = vkEngine->reqAudio_getById(QStringList() << audioInfo);

}

void MessageStyler::styleVideo(VKMessage *mess)
{
    if (!mess->styledText.contains("<*>[[video")){
        emit stylingComplete(curMess);
        return;
    }

    //qDebug() << "RECEIVED: " << mess->styledText;

    //Пока плеер гонит под виндой, избавлюсь от него.
    //хотя вообще достаточно раскомментировать код ниже и наоборот закоментить эту строчку
    //и тогда будет лафа в виде видеоплеера прям внутри чата.

    //styleEmails(curMess);
    emit stylingComplete(curMess);

    /*
    QString title = mess->styledText.left(mess->styledText.indexOf("<*>"));

    QString videoInfo = mess->styledText.right(mess->styledText.count() -
                                               (mess->styledText.lastIndexOf("<*>")+10));

    mess->styledText = title + "<br/>";

    videoInfo = videoInfo.remove("]]");

    getVideoUrlId = vkEngine->reqVideoUrl(videoInfo);
    */

}

void MessageStyler::photoGetByIdFinished(VKRequest *req)
{
    if (req->reqType != VKRequest::Photos_getById ||
        req->reqId != getPhotoById){
        return;
    }

    //qDebug() << req->result;

    QDomDocument doc("PhotoById");
    doc.setContent(req->result);
    QDomElement docElem = doc.documentElement();
    QDomNodeList photos = docElem.elementsByTagName ( "photo" );
    QDomElement photoElem = photos.at(0).toElement();

    QString url = photoElem.elementsByTagName("src").at(0).toElement().text();
    QString url_big = photoElem.elementsByTagName("src_big").at(0).toElement().text();

    QString img = "<a href='Images_urlbig_" + url_big + "'><img src='" + url + "'></a>";
    curMess->styledText += img;

    styleAudio(curMess);

}

void MessageStyler::audioGetByIdFinished(VKRequest *req)
{
    if (req->reqType != VKRequest::Audio_getById ||
        req->reqId != getAudioById){
        return;
    }

    //qDebug() << req->result;

    QDomDocument doc("AudioById");
    doc.setContent(req->result);
    QDomElement docElem = doc.documentElement();
    QDomNodeList audios = docElem.elementsByTagName ( "audio" );
    QDomElement audioElem = audios.at(0).toElement();

    QString url = audioElem.elementsByTagName("url").at(0).toElement().text();

    QString object = "<object type=\"text/vkmp3\" data=\"" + url + "\" width=\"100%\" height=\"50\"></object>";

    //QString object = "<object type='text/vkmp3' data='test.mp3' height='70' width='90'></object>";

    curMess->styledText += object;

    //qDebug() << "Plyat: " << curMess->styledText;

    styleVideo(curMess);

}

void MessageStyler::videoGetUrlFinished(QUrl url, bool isBroken)
{
    //qDebug() << "make object:" << url << isBroken;

    if (!isBroken){

        QString object = "<object type=\"text/vkflv\" data=\"" + url.toString() + "\" width=\"100%\" height=\"300\"></object>";

        curMess->styledText = curMess->styledText.remove(curMess->styledText.indexOf("<div"),
                                                         curMess->styledText.length()-curMess->styledText.indexOf("<div"));

        curMess->styledText += object;
    }

    //qDebug() << curMess->styledText;

    emit stylingComplete(curMess);
}

void MessageStyler::styleLinks(VKMessage *mess)
{
    if (mess->styledText.contains("<*>[[video") ||
        mess->styledText.contains("<*>[[audio")){
        return;
    }

    QString html = mess->styledText;
    static QRegExp linkRegExp("(([a-z]+://|www\\d?\\.)[^\\s]+)");
    int pos = 0;
    while((pos=linkRegExp.indexIn(html, pos)) != -1)
    {
        QString link = linkRegExp.cap(0);
        link = "<a href='" + link + "' target='_blank'>" + link + "</a>";
        html.replace(linkRegExp.cap(0), link);
        pos += link.count();
    }
    mess->styledText = html;
}

void MessageStyler::styleEmails(VKMessage *mess)
{
    QString html = mess->styledText;
    static QRegExp emailRegExp("((?:\\w+\\.)*\\w+@(?:\\w+\\.)*\\w+)");
    emailRegExp.indexIn(html);
    for(int i=0;i<emailRegExp.numCaptures();i++)
    {
        QString email = emailRegExp.cap(i);
        email = "<a href=\"mailto:" + email + "\">" + email + "</a>";
        html.replace(emailRegExp.cap(i), email);
    }
    mess->styledText = html;
}
