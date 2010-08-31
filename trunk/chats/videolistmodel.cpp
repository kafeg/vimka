#include "videolistmodel.h"
#include "../vk/vkengine.h"
#include <QListView>
#include <QDebug>
#include <QDomNode>
#include <QDomNodeList>
#include <QDomElement>
#include "../roster/rosterwidgetitem.h"
#include "../roster/qhttpimage.h"
#include <QDateTime>

VideoListModel::VideoListModel(VKEngine *engine, QListView *rosterView,
                               QObject *parent) :
QStandardItemModel(0, 5, parent)
{
    m_vkEngine = engine;
    m_rosterView = rosterView;
    m_rosterView->setWordWrap (true);
    m_rosterView->setModel(this);

    connect(m_vkEngine, SIGNAL(requestFinished(VKRequest *)),
            this, SLOT(receivedVideo_get(VKRequest *)));
}

void VideoListModel::select()
{
    clear();
    reqVideo_get = m_vkEngine->reqVideo_get(m_vkEngine->mid);
}

void VideoListModel::receivedVideo_get(VKRequest *req)
{
    if (req->reqType != VKRequest::Video_get ||
        req->reqId != reqVideo_get){
        return;
    }

    //qDebug() << req->result;

    QDomDocument doc("Videos");
    doc.setContent(req->result);
    QDomElement docElem = doc.documentElement();
    QDomNodeList items = docElem.elementsByTagName ( "video" );
    for(int i=0; i < items.count(); i++){
        QDomElement itemElem = items.at(i).toElement();

        QString uid = itemElem.elementsByTagName("owner_id").at(0).toElement().text();
        QString vid = itemElem.elementsByTagName("vid").at(0).toElement().text();
        QString title = itemElem.elementsByTagName("title").at(0).toElement().text().replace("&quot;","\"");
        QString description = itemElem.elementsByTagName("description").at(0).toElement().text();
        QString duration = itemElem.elementsByTagName("duration").at(0).toElement().text();
        QString link = itemElem.elementsByTagName("link").at(0).toElement().text();
        QString image = itemElem.elementsByTagName("image").at(0).toElement().text();
        QString date = itemElem.elementsByTagName("date").at(0).toElement().text();

        QString text = title + "\n";
        QTime time(0,0,0);
        QTime durTime = time.addSecs(duration.toInt());

        text += tr("Duration: %1\n").arg(durTime.toString("mm:ss"));
        text += tr("Date: %1\n").arg(QDateTime().addSecs(date.toInt()).toString("dd.MM.yy hh:mm:ss"));
        text += tr("Description: %1").arg(description);

        text = text.replace("&quot;","\"");

        RosterWidgetItem *iconItem = new RosterWidgetItem(text);
        QStandardItem *urlItem = new QStandardItem("http://vkontakte.ru/video"+uid+"_"+vid);
        QStandardItem *titleItem = new QStandardItem(title);
        QStandardItem *iconUrlItem = new QStandardItem(image);
        QStandardItem *vidItem = new QStandardItem(vid);

        QHttpImage *httpImage = new QHttpImage(this);
        httpImage->setUrl(image);
        connect(httpImage, SIGNAL(loadedIcon(QIcon)),
                iconItem,SLOT(applyIcon(QIcon)));
        httpImage->load();

        appendRow(QList<QStandardItem *>() << iconItem << urlItem << titleItem
                  << iconUrlItem << vidItem);
    }

    emit selected();
}
