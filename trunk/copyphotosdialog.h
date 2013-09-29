#ifndef COPYPHOTOSDIALOG_H
#define COPYPHOTOSDIALOG_H

#include <QDialog>

namespace Ui {
    class CopyPhotosDialog;
}

class VKRequest;
class VKEngine;
class QNetworkReply;

class CopyPhotosDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CopyPhotosDialog(VKEngine *vkEngine, QWidget *parent = 0);
    ~CopyPhotosDialog();

private slots:
    void receivedPhotos_getUploadServer(VKRequest *req);
    void receivedPhotos_get(VKRequest *req);
    void receivedPhotos_getInTarget(VKRequest *req);
    void receivedPhotos_move(VKRequest *req);

    void on_pbStart_clicked();
    void downloadError();
    void downloadFinished();
    void uploadError();
    void uploadFinished();

private:
    Ui::CopyPhotosDialog *ui;
    VKEngine *m_vkEngine;
    int reqGetUploadServer;
    int reqPhotosGet;
    int reqPhotosGetInTarget;
    QString aidFrom;
    QString gidFrom;
    QString aidTo;
    QString gidTo;
    bool withComments;
    bool isFromGroup;
    QString albumDir;
    QString uploadServer;

    struct VKAlbumPhoto {
        QString pid;
        QString aid;
        QString owner_id;
        QString src;
        QString src_big;
        QString src_small;
        QString text;
        QString src_xbig;
        QString src_xxbig;
        QString cacheFileName;
    };

    QList<VKAlbumPhoto> photos;
    QList<VKAlbumPhoto> photosInTarget;

    void uploadNextPhoto();
    void downloadNextPhoto();
    int currentPhoto;
    QNetworkReply *currentReply;
    void plog(QString line);

    QByteArray encodeMultipartFormdata(QStringList files);
    QString content_type;
    int reqMovePhoto;
};

#endif // COPYPHOTOSDIALOG_H
