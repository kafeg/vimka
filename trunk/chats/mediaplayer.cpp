#include "mediaplayer.h"
#include <QWidget>
#include <QToolButton>
#include <QStyle>
#include <QHBoxLayout>
#include <QMapIterator>
#include <QApplication>
#include <QKeyEvent>
#include "../settingsmanager.h"

MediaPlayer::MediaPlayer(QObject *parent, SettingsManager *sMn) :
        QObject(parent), m_AudioOutput(Phonon::MusicCategory)
        //, m_AudioNotificationOutput(Phonon::CommunicationCategory)
{
    m_sMn = sMn;
    //m_videoWidget = new MediaVideoWidget(this);
    //m_videoWidget->setMinimumWidth(300);
    //m_videoWidget->setMinimumHeight(220);

    m_MediaObject.stop ();
    //m_MediaNotificationObject.stop();

    m_audioOutputPath = Phonon::createPath(&m_MediaObject, &m_AudioOutput);
    //m_audioNotificationOutputPath = Phonon::createPath(&m_MediaNotificationObject, &m_AudioNotificationOutput);

    //Phonon::createPath(&m_MediaObject, m_videoWidget);

    //qDebug() << m_sMn->styleDir()+"/sounds/incomming.mp3";
    //m_MediaNotificationObject.setCurrentSource(Phonon::MediaSource(m_sMn->styleDir()+"/sounds/incomming.mp3"));
    //m_MediaNotificationObject.setCurrentSource(Phonon::MediaSource(QApplication::applicationDirPath()+"/inc.mp3"));
    //m_MediaObject.setCurrentSource(Phonon::MediaSource(QApplication::applicationDirPath()+"/inc.mp3"));
    //m_MediaNotificationObject.play();
    //m_MediaNotificationObject.play();
    //m_MediaObject.play();

    //m_MediaObject.setCurrentSource(Phonon::MediaSource(QApplication::applicationDirPath()+"/inc.mp3"));
    //m_MediaObject.setCurrentSource(Phonon::MediaSource(m_sMn->styleDir()+"/sounds/incomming.mp3"));

    //qDebug() << m_MediaObject.currentSource().fileName();
    //m_MediaObject.play();
}

void MediaPlayer::incomming()
{
    if (m_MediaObject.state() != Phonon::PlayingState){
        //m_MediaObject.setCurrentSource(Phonon::MediaSource(QApplication::applicationDirPath()+"/inc.mp3"));
        m_MediaObject.setCurrentSource(Phonon::MediaSource(m_sMn->styleDir()+"/sounds/incomming.mp3"));
        m_MediaObject.play();
    }
}

MediaPlayer::~MediaPlayer()
{
    m_MediaObject.stop ();
}

QWidget *MediaPlayer::getMusicPlayer(QUrl url){

    QWidget *musWgt = new  QWidget();
    musWgt->setAttribute(Qt::WA_NoSystemBackground);
    musWgt->setMinimumSize(50,30);
    QHBoxLayout *layout = new QHBoxLayout(musWgt);

    QToolButton *playButton = new QToolButton(musWgt);
    playIcon = m_sMn->play;
    pauseIcon = m_sMn->pause;
    playButton->setIcon(playIcon);
    playButton->setObjectName("playButton");
    playButton->setAutoRaise(true);

    Phonon::SeekSlider *slider = new Phonon::SeekSlider(musWgt);
    slider->setMediaObject(&m_MediaObject);
    Phonon::VolumeSlider *volume = new Phonon::VolumeSlider(&m_AudioOutput);

    layout->addWidget(playButton);
    layout->addWidget(slider);
    layout->addWidget(volume);

    layout->addStretch();

    playButtons.insert(url.toString(), playButton);
    playSliders.insert(url.toString(), slider);
    playVolumes.insert(url.toString(), volume);

    if (m_MediaObject.currentSource().url() != url){
        playSliders.value(url.toString())->hide();
        playVolumes.value(url.toString())->hide();
    }

    //m_MediaObject.stop();
    return musWgt;
}

void MediaPlayer::clearButtons()
{
    playButtons.clear();
    playSliders.clear();
}

QWidget *MediaPlayer::getVideoPlayer(QUrl url){

    QWidget *videoWgt = new  QWidget();
    videoWgt->setAttribute(Qt::WA_NoSystemBackground);

    QVBoxLayout *vLayout = new QVBoxLayout(videoWgt);

    QWidget *hVid = new  QWidget();
    QHBoxLayout *hVidLayout = new QHBoxLayout(hVid);

    //hVidLayout->addStretch();
    //hVidLayout->addWidget(m_videoWidget);
    hVidLayout->addStretch();

    vLayout->addWidget(hVid);

    QWidget *hCont = new  QWidget();
    QHBoxLayout *layout = new QHBoxLayout(hCont);

    vLayout->addWidget(hCont);

    QToolButton *playButton = new QToolButton(hCont);
    //playIcon = hCont->style()->standardIcon(QStyle::SP_MediaPlay);
    //pauseIcon = hCont->style()->standardIcon(QStyle::SP_MediaPause);
    playIcon = m_sMn->play;
    pauseIcon = m_sMn->pause;
    playButton->setIcon(playIcon);
    playButton->setObjectName("playButton");

    Phonon::SeekSlider *slider = new Phonon::SeekSlider(videoWgt);
    slider->setMediaObject(&m_MediaObject);
    Phonon::VolumeSlider *volume = new Phonon::VolumeSlider(&m_AudioOutput);

    layout->addWidget(playButton);
    layout->addWidget(slider);
    layout->addWidget(volume);

    layout->addStretch();

    playButtons.insert(url.toString(), playButton);
    playSliders.insert(url.toString(), slider);
    playVolumes.insert(url.toString(), volume);

    if (m_MediaObject.currentSource().url() != url){
        playSliders.value(url.toString())->hide();
        playVolumes.value(url.toString())->hide();
    }
    //m_MediaObject.stop();
    return videoWgt;

}

void MediaPlayer::play(QUrl url)
{
    if (m_MediaObject.currentSource().url() != url){
       //qDebug() << "Load and Play " << url;
        m_MediaObject.stop();
        m_MediaObject.setCurrentSource(Phonon::MediaSource(url));
        m_MediaObject.play();
        playButtons.value(url.toString())->setIcon(pauseIcon);

        QMapIterator<QString, QToolButton *> i1(playButtons);
        while(i1.hasNext()){
            i1.next();
            if (i1.key() != url.toString()){
                i1.value()->setIcon(playIcon);
            }
            QApplication::processEvents();
        }

        QMapIterator<QString, Phonon::SeekSlider *> i2(playSliders);
        while(i2.hasNext()){
            i2.next();
            if (i2.key() != url.toString()){
                i2.value()->hide();
            }
            QApplication::processEvents();
        }

        QMapIterator<QString, Phonon::VolumeSlider *> i3(playVolumes);
        while(i3.hasNext()){
            i3.next();
            if (i3.key() != url.toString()){
                i3.value()->hide();
            }
            QApplication::processEvents();
        }

    }else{
        if (m_MediaObject.state() == Phonon::PlayingState){
            m_MediaObject.pause();
            playButtons.value(url.toString())->setIcon(playIcon);
        } else {
            if (m_MediaObject.currentTime() == m_MediaObject.totalTime()){
                m_MediaObject.seek(0);
            }
            m_MediaObject.play();
            playButtons.value(url.toString())->setIcon(pauseIcon);
        }
    }

    playSliders.value(url.toString())->show();
    playVolumes.value(url.toString())->show();

}

//видео виджет

MediaVideoWidget::MediaVideoWidget(MediaPlayer *player, QWidget *parent) :
     Phonon::VideoWidget(parent), m_player(player), m_action(this)
 {
     m_action.setCheckable(true);
     m_action.setChecked(false);
     m_action.setShortcut(QKeySequence( Qt::AltModifier + Qt::Key_Return));
     m_action.setShortcutContext(Qt::WindowShortcut);
     connect(&m_action, SIGNAL(toggled(bool)), SLOT(setFullScreen(bool)));
     addAction(&m_action);
     setAcceptDrops(true);
 }

 void MediaVideoWidget::setFullScreen(bool enabled)
 {
     Phonon::VideoWidget::setFullScreen(enabled);
     emit fullScreenChanged(enabled);
 }

 void MediaVideoWidget::mouseDoubleClickEvent(QMouseEvent *e)
 {
     Phonon::VideoWidget::mouseDoubleClickEvent(e);
     setFullScreen(!isFullScreen());
 }

 void MediaVideoWidget::keyPressEvent(QKeyEvent *e)
 {
     if(!e->modifiers()) {
         // On non-QWERTY Symbian key-based devices, there is no space key.
         // The zero key typically is marked with a space character.
         if (e->key() == Qt::Key_Space || e->key() == Qt::Key_0) {
             //m_player->playPause();
             e->accept();
             return;
         }

         // On Symbian devices, there is no key which maps to Qt::Key_Escape
         // On devices which lack a backspace key (i.e. non-QWERTY devices),
         // the 'C' key maps to Qt::Key_Backspace
         else if (e->key() == Qt::Key_Escape || e->key() == Qt::Key_Backspace) {
             setFullScreen(false);
             e->accept();
             return;
         }
     }
     Phonon::VideoWidget::keyPressEvent(e);
 }

 bool MediaVideoWidget::event(QEvent *e)
 {
     switch(e->type())
     {
     case QEvent::Close:
         //we just ignore the cose events on the video widget
         //this prevents ALT+F4 from having an effect in fullscreen mode
         e->ignore();
         return true;
     case QEvent::MouseMove:
 #ifndef QT_NO_CURSOR
         unsetCursor();
 #endif
         //fall through
     case QEvent::WindowStateChange:
         {
             //we just update the state of the checkbox, in case it wasn't already
             m_action.setChecked(windowState() & Qt::WindowFullScreen);
             //const Qt::WindowFlags flags = m_player->windowFlags();
             if (windowState() & Qt::WindowFullScreen) {
                 m_timer.start(1000, this);
             } else {
                 m_timer.stop();
 #ifndef QT_NO_CURSOR
                 unsetCursor();
 #endif
             }
         }
         break;
     default:
         break;
     }

     return Phonon::VideoWidget::event(e);
 }

 void MediaVideoWidget::timerEvent(QTimerEvent *e)
 {
     if (e->timerId() == m_timer.timerId()) {
         //let's store the cursor shape
 #ifndef QT_NO_CURSOR
         setCursor(Qt::BlankCursor);
 #endif
     }
     Phonon::VideoWidget::timerEvent(e);
 }

