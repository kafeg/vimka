#ifndef VIDEOLISTMODEL_H
#define VIDEOLISTMODEL_H

#include <QStandardItemModel>

class VKEngine;
class QListView;
class VKRequest;

class VideoListModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit VideoListModel(VKEngine *engine, QListView *rosterView,
                            QObject *parent = 0);

    void select();
signals:
    void selected();

public slots:

private:
    VKEngine *m_vkEngine;
    QListView *m_rosterView;
    int reqVideo_get;

private slots:
    void receivedVideo_get(VKRequest *req);
};

#endif // VIDEOLISTMODEL_H
