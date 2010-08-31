#ifndef PHOTOSQMLVIEW_H
#define PHOTOSQMLVIEW_H

#include <QObject>
#include <QDeclarativeView>

class VKEngine;
class AlbumsModel;

class PhotosQmlView : public QObject
{
    Q_OBJECT
public:
    explicit PhotosQmlView(VKEngine *engine );

    QDeclarativeView view;

signals:

public slots:

private:
    VKEngine *vkEngine;
    AlbumsModel *albumsModel;

};

#endif // PHOTOSQMLVIEW_H
