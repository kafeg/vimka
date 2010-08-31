#ifndef QHTTPIMAGE_H
#define QHTTPIMAGE_H

#include <QObject>
#include <QBuffer>
#include <QHttp>
#include <QUrl>
#include <QImage>
#include <QIcon>

class QHttpImage : public QObject
{
    Q_OBJECT
public:
    explicit QHttpImage(QObject *parent, QUrl url = QUrl());

    void load();
    QImage getImage();
    QImage getOriginalImage();

    QIcon getIcon();
    QIcon getOriginalIcon();

    void setUrl(QUrl url) { m_url = url; }
signals:
    void loaded();
    void loadedIcon(QIcon icon);
    void loadedOriginalIcon(QIcon icon);

private slots:
    void finished(int requestId, bool error);

private:
     QBuffer *buffer;
     QByteArray bytes;
     QHttp *http;
     int Request;
     QUrl m_url;
     QImage img;

};

#endif // QHTTPIMAGE_H
