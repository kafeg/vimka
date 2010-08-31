#ifndef ALBUMSMODEL_H
#define ALBUMSMODEL_H

#include <QAbstractListModel>

class AlbumsModelItem;
class VKEngine;
class VKRequest;

class AlbumsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit AlbumsModel(VKEngine *engine);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    void select();
signals:
    void photoIsSelected(QString preview, QString hq,
                         QString pid, QString aid, QString mid,
                         QString fromId);
public slots:
    void photoSelected(QString preview, QString hq,
                       QString pid, QString aid, QString mid,
                       QString fromId);

private:
    Q_DISABLE_COPY(AlbumsModel);

    QList<AlbumsModelItem *> items;
    enum ListMenuItemRoles
    {
        TitleRole = Qt::UserRole+1,
        UrlRole
    };

    VKEngine *vkEngine;
    int getAlbumsId;
    VKRequest *urlMaker;
    QString urlApi;

private slots:
    void getAlbumsFinished(VKRequest *req);

};

#endif // ALBUMSMODEL_H
