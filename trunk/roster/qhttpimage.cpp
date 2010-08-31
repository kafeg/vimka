#include "qhttpimage.h"
#include <QDebug>

QHttpImage::QHttpImage(QObject *parent, QUrl url) :
    QObject(parent)
{
    http = new QHttp(this);
    http->ignoreSslErrors();
    connect(http, SIGNAL(requestFinished(int, bool)),this, SLOT(finished(int,
    bool)));
    m_url = url;
    buffer = new QBuffer(&bytes);
}

void QHttpImage::load()
{
    buffer->open(QIODevice::WriteOnly | QIODevice::Truncate);
    http->setHost(m_url.host());
    //qDebug() << m_url.path();
    Request=http->get (m_url.path(),buffer);
}

void QHttpImage::finished(int requestId, bool error)
{
    if (Request==requestId){
        img.loadFromData(bytes);
        //img = img.scaledToWidth(32);
    }

    //qDebug() << m_url;

    if (error){
        qDebug() << http->errorString() << http->lastResponse().toString();
        load();
        return;
    }

    emit loaded();
    emit loadedIcon(getIcon());
    emit loadedOriginalIcon(getOriginalIcon());
}

QImage QHttpImage::getImage()
{
    return img.copy(0,0,img.width(),img.width());
}

QImage QHttpImage::getOriginalImage()
{
    return img;
}

QIcon QHttpImage::getIcon()
{
    QIcon icon;
    icon.addPixmap(QPixmap::fromImage(getImage()), QIcon::Normal, QIcon::On);
    return icon;
}

QIcon QHttpImage::getOriginalIcon()
{
    QIcon icon;
    icon.addPixmap(QPixmap::fromImage(getOriginalImage()),
                   QIcon::Normal, QIcon::On);
    return icon;
}
