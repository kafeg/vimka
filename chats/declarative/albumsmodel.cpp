#include "albumsmodel.h"
#include "../../vk/vkengine.h"
#include "../../vk/vkrequest.h"
#include <QDebug>
#include <QUrl>
#include <QDomNode>
#include <QDomNodeList>
#include <QDomElement>
#include <QApplication>

//элемент
class AlbumsModelItem
{
public:
    AlbumsModelItem(QString _name, QUrl _url)
        : name(_name), url(_url)
    {}

    QString name;
    QUrl url;
};

//модель
AlbumsModel::AlbumsModel(VKEngine *engine) :
        QAbstractListModel(engine)
{
    vkEngine = engine;

    connect(vkEngine, SIGNAL(requestFinished(VKRequest *)),
            this, SLOT(getAlbumsFinished(VKRequest *)));

    QHash<int, QByteArray> roles = roleNames();
    roles.insert(TitleRole, QByteArray("title"));
    roles.insert(UrlRole, QByteArray("url"));
    setRoleNames(roles);

    urlMaker = vkEngine->getEmptyReq(VKRequest::Photos_get);

    urlApi = "http://api.vkontakte.ru/api.php?";
}

void AlbumsModel::select()
{
    getAlbumsId = vkEngine->reqPhotos_getAlbums(vkEngine->mid);

}

QVariant AlbumsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()){
        return QVariant();
    }
    if (index.row() > (items.size()-1) ){
        return QVariant();
    }
    AlbumsModelItem *item = items.at(index.row());
    switch (role)
    {
    case Qt::DisplayRole:
    case TitleRole:
        return QVariant::fromValue(item->name);
    case UrlRole:
        return QVariant::fromValue(item->url.toString());
    default:
        return QVariant();
    }
}

int AlbumsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return items.size();
}

void AlbumsModel::getAlbumsFinished(VKRequest *req)
{
    //qDebug() << req->result;

    if(req->reqType != VKRequest::Photos_getAlbums //||
      // req->reqId != getAlbumsId
       ){
        return;
    }

    items.clear();

    //qDebug() << req->result;

    QDomDocument doc("Albums");
    doc.setContent(req->result);
    QDomElement docElem = doc.documentElement();
    QDomNodeList albums = docElem.elementsByTagName ( "album" );
    for(int i=0; i < albums.count(); i++){
        QDomElement albumElem = albums.at(i).toElement();

        QString title = albumElem.elementsByTagName("title").at(0).toElement().text();
        QString aid = albumElem.elementsByTagName("aid").at(0).toElement().text();

        urlMaker->make_Photos_get(vkEngine->mid, aid);

        AlbumsModelItem *item = new AlbumsModelItem(title, urlApi + urlMaker->requestUrlPath);

        items << item;

        QApplication::processEvents();

        //qDebug() << item->url.toString();
    }

}

void AlbumsModel::photoSelected(QString preview, QString hq,
                                QString pid, QString aid, QString mid,
                                QString fromId)
{
    //qDebug() << "Photo selected: " << preview << hq << fromId;
    emit photoIsSelected (preview, hq, pid, aid, mid, fromId);
}
