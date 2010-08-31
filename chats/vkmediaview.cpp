#include "vkmediaview.h"
#include <QHBoxLayout>
#include "mediaplayer.h"
#include <QToolButton>

VKMediaView::VKMediaView(const QString &mimeType, QUrl url,
                         MediaPlayer *pagePlayer, QWidget *parent) :
QWidget(parent)
{
    this->mimeType = mimeType;
    this->url = url;
    this->pagePlayer = pagePlayer;

    QHBoxLayout *layout = new QHBoxLayout(this);

    if (this->mimeType == "text/vkmp3"){
        layout->addWidget(pagePlayer->getMusicPlayer(url));
        connect(pagePlayer->playButtons.value(url.toString()), SIGNAL(clicked()), this, SLOT(playMusic()));
        //playMusic();
    }else if (this->mimeType == "text/vkflv"){
        layout->addWidget(pagePlayer->getVideoPlayer(url));
        connect(pagePlayer->playButtons.value(url.toString()), SIGNAL(clicked()), this, SLOT(playVideo()));
    }

}

void VKMediaView::playMusic()
{
    pagePlayer->play(url);
}

void VKMediaView::playVideo()
{
    pagePlayer->play(url);
}
