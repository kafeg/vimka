#include "photosqmlview.h"
#include <QDeclarativeContext>
#include "vk/vkengine.h"
#include "albumsmodel.h"

PhotosQmlView::PhotosQmlView(VKEngine *engine) :
        QObject(engine)
{
    vkEngine = engine;
    //QStringList albumsUrls;
    //albumsUrls.append("Wildlife");
    //albumsUrls.append("Flowers");

    albumsModel = new AlbumsModel(vkEngine);

    view.rootContext()->setContextProperty("photosModel", albumsModel);
    view.setResizeMode ( QDeclarativeView::SizeRootObjectToView );
    view.setSource(QUrl("qrc:/declarative/photoviewer/photoviewer.qml"));

    albumsModel->select();

}
