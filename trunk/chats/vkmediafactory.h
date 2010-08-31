#ifndef VKMEDIAFACTORY_H
#define VKMEDIAFACTORY_H

#include <QWebPluginFactory>
#include <QNetworkAccessManager>

class MediaPlayer;

class VKMediaFactory : public QWebPluginFactory
{
    Q_OBJECT
public:
    explicit VKMediaFactory(QObject *parent, MediaPlayer *pagePlayer);

    QObject *create(const QString &mimeType,
                    const QUrl &url, const QStringList &argumentNames,
                    const QStringList &argumentValues) const;
    QList<QWebPluginFactory::Plugin> plugins() const;

private:
    MediaPlayer *pagePlayer;

};

#endif // VKMEDIAFACTORY_H
