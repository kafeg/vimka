#include "vkmediafactory.h"
#include <QDebug>
#include <QUrl>
#include <QWidget>
#include "vkmediaview.h"
#include "mediaplayer.h"

//этот класс - позволяет на встраивать собственные виджеты прям в тело web-страницы вебкита.

VKMediaFactory::VKMediaFactory(QObject *parent, MediaPlayer *pagePlayer) :
        QWebPluginFactory(parent)
{
    this->pagePlayer = pagePlayer;
}

QList<QWebPluginFactory::Plugin> VKMediaFactory::plugins() const
{
    QWebPluginFactory::MimeType mimeType;
    mimeType.name = "text/vkmp3";
    mimeType.description = "Comma-separated values";
    mimeType.fileExtensions = QStringList() << "mp3";

    QWebPluginFactory::MimeType mimeType2;
    mimeType.name = "text/vkflv";
    mimeType.description = "Comma-separated values";
    mimeType.fileExtensions = QStringList() << "flv";

    QWebPluginFactory::Plugin plugin;
    plugin.name = "VKontakte mp3 player";
    plugin.description = "A VKontakte mp3/flv Web plugin.";
    plugin.mimeTypes = QList<MimeType>() << mimeType << mimeType2;

    return QList<QWebPluginFactory::Plugin>() << plugin;
}

QObject *VKMediaFactory::create(const QString &mimeType,
                                const QUrl &url, const QStringList &argumentNames,
                                const QStringList &argumentValues) const
{
    //qDebug() << mimeType << url;

    if (mimeType != "text/vkmp3" && mimeType != "text/vkflv")
        return 0;

    VKMediaView *view = new VKMediaView(argumentValues[argumentNames.indexOf("type")],
                                        url, pagePlayer);

    return view;
}
