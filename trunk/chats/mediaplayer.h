#ifndef MEDIAPLAYER_H
#define MEDIAPLAYER_H

#include <QObject>
#include <MediaObject>
#include <AudioOutput>
#include <SeekSlider>
#include <VolumeSlider>
#include <VideoWidget>
#include <QUrl>
#include <QIcon>
#include <QMap>
#include <QBasicTimer>
#include <QAction>

class SeekSlider;
class QToolButton;
class MediaPlayer;
class SettingsManager;

class MediaVideoWidget : public Phonon::VideoWidget
 {
     Q_OBJECT

 public:
     MediaVideoWidget(MediaPlayer *player, QWidget *parent = 0);

 public slots:
     // Over-riding non-virtual Phonon::VideoWidget slot
     void setFullScreen(bool);

 signals:
     void fullScreenChanged(bool);

 protected:
     void mouseDoubleClickEvent(QMouseEvent *e);
     void keyPressEvent(QKeyEvent *e);
     bool event(QEvent *e);
     void timerEvent(QTimerEvent *e);

 private:
     MediaPlayer *m_player;
     QBasicTimer m_timer;
     QAction m_action;
 };

class MediaPlayer : public QObject
{
    Q_OBJECT

    enum MediaType {
        Video,
        Audio
    } mediaType;

public:
    MediaPlayer(QObject *parent, SettingsManager *sMn);
    ~MediaPlayer();

    //Phonon::MediaObject m_MediaNotificationObject;
    Phonon::MediaObject m_MediaObject;

    //Phonon::AudioOutput m_AudioNotificationOutput;
    Phonon::AudioOutput m_AudioOutput;

    //Phonon::Path m_audioNotificationOutputPath;
    Phonon::Path m_audioOutputPath;

    //MediaVideoWidget *m_videoWidget;
    SettingsManager *m_sMn;

    QWidget *getMusicPlayer(QUrl url);
    QWidget *getVideoPlayer(QUrl url);

    void play(QUrl url);
    void incomming();

    QIcon playIcon;
    QIcon pauseIcon;

    QMap<QString, QToolButton *> playButtons;
    QMap<QString, Phonon::SeekSlider *> playSliders;
    QMap<QString, Phonon::VolumeSlider *> playVolumes;

signals:

public slots:
    void clearButtons();


private:


};

#endif // MEDIAPLAYER_H
