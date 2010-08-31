#ifndef VKMEDIAVIEW_H
#define VKMEDIAVIEW_H

#include <QWidget>
#include <QUrl>

class MediaPlayer;

class VKMediaView : public QWidget
{
    Q_OBJECT

public:
    explicit VKMediaView(const QString &mimeType, QUrl url,
                         MediaPlayer *pagePlayer, QWidget *parent = 0);

signals:

public slots:

private:
    QString mimeType;
    QUrl url;
    MediaPlayer *pagePlayer;

private slots:
    void playMusic();
    void playVideo();

};

#endif // VKMEDIAVIEW_H
