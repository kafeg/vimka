#include "copyphotosdialog.h"
#include "ui_copyphotosdialog.h"
#include "vk/vkengine.h"
#include <QDebug>
#include <QDomDocument>
#include <QDir>
#include <QNetworkAccessManager>
#include <QNetworkReply>

CopyPhotosDialog::CopyPhotosDialog(VKEngine *vkEngine, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CopyPhotosDialog),
    m_vkEngine(vkEngine),
    reqPhotosGetInTarget(-1),
    reqPhotosGet(-1)
{
    ui->setupUi(this);

    connect(m_vkEngine, SIGNAL(requestFinished(VKRequest *)),
            this, SLOT(receivedPhotos_getUploadServer(VKRequest *)));
    connect(m_vkEngine, SIGNAL(requestFinished(VKRequest *)),
            this, SLOT(receivedPhotos_get(VKRequest *)));
    connect(m_vkEngine, SIGNAL(requestFinished(VKRequest *)),
            this, SLOT(receivedPhotos_getInTarget(VKRequest *)));
    connect(m_vkEngine, SIGNAL(requestFinished(VKRequest *)),
            this, SLOT(receivedPhotos_move(VKRequest *)));
}

CopyPhotosDialog::~CopyPhotosDialog()
{
    delete ui;
}

void CopyPhotosDialog::plog(QString line)
{
    ui->pteLog->appendPlainText(line);
}

void CopyPhotosDialog::on_pbStart_clicked()
{
    ui->pteLog->clear();
    plog(tr("Starting..."));

    aidFrom = ui->leAidFrom->text();
    gidFrom = ui->leGidFrom->text();
    aidTo = ui->leAidTo->text();
    gidTo = ui->cbToGroup->isChecked() ? ui->leGidTo->text() : "";
    withComments = ui->cbWithComments->isChecked();
    isFromGroup = ui->cbFromGroup->isChecked();
    albumDir = "/tmp/vk_album"+gidFrom+"_"+aidFrom;

    plog(tr("Get upload server..."));
    reqGetUploadServer = m_vkEngine->reqPhotos_getUploadServer(aidTo, gidTo);
}

void CopyPhotosDialog::receivedPhotos_getUploadServer(VKRequest *req)
{

    if((req->reqType!=VKRequest::Photos_getUploadServer) ||
            (reqGetUploadServer != req->reqId)){
        return;
    }

    if (req->result.contains("<error_code>114</error_code>")){
        ui->pteLog->appendPlainText(tr("Wrong 'To' album!"));
    }else if (req->result.contains("<error_code>200</error_code>")){
        ui->pteLog->appendPlainText(tr("Access denied!"));
    }else{
        int start = req->result.indexOf("<upload_url>")+QString("<upload_url>").length();
        int count = req->result.indexOf("</upload_url>") - start;
        uploadServer = req->result.mid(start, count);
        plog(tr("Upload server: %1").arg(uploadServer));

        reqPhotosGetInTarget = m_vkEngine->reqPhotos_get(gidTo, aidTo);
    }
}

void CopyPhotosDialog::receivedPhotos_getInTarget(VKRequest *req)
{
    if((req->reqType!=VKRequest::Photos_get) ||
            (reqPhotosGetInTarget != req->reqId)){
        return;
    }
    reqPhotosGetInTarget = -1;
    plog(tr("Get target album photos description"));

    photosInTarget.clear();

    QDomDocument doc("Photos");
    doc.setContent(req->result);
    QDomElement docElem = doc.documentElement();
    QDomNodeList users = docElem.elementsByTagName ( "photo" );
    for(int i=0; i < users.count(); i++){
        QDomElement userElem = users.at(i).toElement();

        VKAlbumPhoto photo;

        photo.pid = userElem.elementsByTagName("pid").at(0).toElement().text();
        photo.aid = userElem.elementsByTagName("aid").at(0).toElement().text();
        photo.owner_id = userElem.elementsByTagName("owner_id").at(0).toElement().text();
        photo.src = userElem.elementsByTagName("src").at(0).toElement().text();
        photo.src_big = userElem.elementsByTagName("src_big").at(0).toElement().text();
        photo.src_small = userElem.elementsByTagName("src_small").at(0).toElement().text();
        photo.text = userElem.elementsByTagName("text").at(0).toElement().text();
        photo.src_xbig = userElem.elementsByTagName("src_xbig").at(0).toElement().text();
        photo.src_xxbig = userElem.elementsByTagName("src_xxbig").at(0).toElement().text();
        photo.cacheFileName = albumDir + photo.src_big.right(photo.src_big.length() - (photo.src_big.lastIndexOf("/")));

        photosInTarget.append( photo );
    }

    reqPhotosGet = m_vkEngine->reqPhotos_get(gidFrom, aidFrom);
}

void CopyPhotosDialog::receivedPhotos_get(VKRequest *req)
{
    if((req->reqType!=VKRequest::Photos_get) ||
            (reqPhotosGet != req->reqId)){
        return;
    }
    reqPhotosGet = -1;
    plog(tr("Get source album photos description"));

    photos.clear();

    QDomDocument doc("Photos");
    doc.setContent(req->result);
    QDomElement docElem = doc.documentElement();
    QDomNodeList users = docElem.elementsByTagName ( "photo" );
    for(int i=0; i < users.count(); i++){
        QDomElement userElem = users.at(i).toElement();

        VKAlbumPhoto photo;

        photo.pid = userElem.elementsByTagName("pid").at(0).toElement().text();
        photo.aid = userElem.elementsByTagName("aid").at(0).toElement().text();
        photo.owner_id = userElem.elementsByTagName("owner_id").at(0).toElement().text();
        photo.src = userElem.elementsByTagName("src").at(0).toElement().text();
        photo.src_big = userElem.elementsByTagName("src_big").at(0).toElement().text();
        photo.src_small = userElem.elementsByTagName("src_small").at(0).toElement().text();
        photo.text = userElem.elementsByTagName("text").at(0).toElement().text();
        photo.src_xbig = userElem.elementsByTagName("src_xbig").at(0).toElement().text();
        photo.src_xxbig = userElem.elementsByTagName("src_xxbig").at(0).toElement().text();
        photo.cacheFileName = albumDir + photo.src_big.right(photo.src_big.length() - (photo.src_big.lastIndexOf("/")));
        qDebug() << photo.cacheFileName;

        photos.append( photo );
    }
    currentPhoto = 0;

    plog(tr("Finded %1 photos").arg(photos.count()));
    plog(tr("Caching all photos to local filesystem"));

    QDir cacheDir(albumDir);
    if (!cacheDir.exists(cacheDir.path())){
        cacheDir.mkpath(cacheDir.path());
    }
    downloadNextPhoto();
}

void CopyPhotosDialog::downloadNextPhoto()
{
    if (currentPhoto==photos.count()){
        currentPhoto = 0;
        plog(tr("All photos cached! Start uploading..."));
        uploadNextPhoto();
        return;
    }

    VKAlbumPhoto photo = photos.at(currentPhoto);

    QFile photoFile(photo.cacheFileName);
    if (photoFile.exists()){
        downloadError();
        //plog(tr("Alredy exists..."));
        return;
    }

    plog(tr("Download %1").arg(photo.src_big));
    QNetworkAccessManager *nam   = new QNetworkAccessManager( this );
    currentReply = nam->get( QNetworkRequest( QUrl( photo.src_big ) ) );
    connect(currentReply,SIGNAL(finished()),this,SLOT(downloadFinished()));
    connect(currentReply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(downloadError()));
}

void CopyPhotosDialog::downloadFinished()
{
    //plog(tr("Done..."));
    VKAlbumPhoto photo = photos.at(currentPhoto);
    QFile photoFile(photo.cacheFileName);
    if (!photoFile.open(QIODevice::WriteOnly))
        return;
    photoFile.write(currentReply->readAll());
    photoFile.close();
    currentPhoto++;
    downloadNextPhoto();
}

void CopyPhotosDialog::downloadError()
{
    currentPhoto++;
    downloadNextPhoto();
}

QByteArray CopyPhotosDialog::encodeMultipartFormdata(QStringList files){

    QString boundary = "-------------Boundary_$";

    QString CRLF="\r\n";

    QByteArray data;

    int count = 1;
    foreach (QString file, files){
        QFile f(file);
        if (!f.open(QIODevice::ReadOnly))
            return 0;

        data.append("--" + boundary);
        data.append(CRLF);
        data.append("Content-Disposition: form-data; name=\"file"+QString::number(count)+"\"; filename=\""+f.fileName()+"\"");
        data.append(CRLF);
        data.append(CRLF);
        data.append(f.readAll());
        data.append(CRLF);
        f.close();
        count++;
    }

    data.append("--" + boundary + "--");
    data.append(CRLF);
    data.append(CRLF);

    content_type = "multipart/form-data; boundary="+boundary;
    qDebug() << data;
    return data;
}

void CopyPhotosDialog::uploadNextPhoto()
{
    plog(tr("Upload next photo"));

    QNetworkAccessManager *nam   = new QNetworkAccessManager( this );
    VKAlbumPhoto photo = photos.at(currentPhoto);

    QString boundary = "---------------------------723690991551375881941828858";

    QByteArray data(QString("--" + boundary + "\r\n").toAscii());
    data += "Content-Disposition: form-data; name=\"file1\"; filename=\""+photo.cacheFileName.right(photo.cacheFileName.length() -
                                                                                                    (photo.cacheFileName.lastIndexOf("/")+1))+"\"\r\n";
    data += "Content-Type: image/jpeg\r\n\r\n";
    QFile file1(photo.cacheFileName);
    if (!file1.open(QIODevice::ReadOnly))
        return;
    data += file1.readAll();
    data += "\r\n";
    file1.close();

    data += QString("--" + boundary + "--\r\n").toAscii();

    QNetworkRequest req;
    req.setUrl(QUrl( uploadServer ));

    req.setRawHeader("Content-Type", QString("multipart/form-data; boundary=" + boundary).toAscii());
    req.setRawHeader("Content-Length", QString::number(data.length()).toAscii());

    currentReply = nam->post( req, data );
    connect(currentReply,SIGNAL(finished()),this,SLOT(uploadFinished()));
    connect(currentReply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(uploadError()));

    qDebug() << "Length: " + QString::number(data.length());
    plog(data);

    qDebug() << req.rawHeaderList() << req.rawHeader("Content-Type") << req.rawHeader("Content-Length");

    //VKAlbumPhoto photo = photos.at(currentPhoto);
    //reqMovePhoto = m_vkEngine->reqPhotos_move(photo.pid, aidTo, photo.owner_id);
}

void CopyPhotosDialog::receivedPhotos_move(VKRequest *req)
{
    if((req->reqType!=VKRequest::Photos_move) ||
            (reqMovePhoto != req->reqId)){
        return;
    }
    reqMovePhoto = -1;

    qDebug() << req->result;
}

void CopyPhotosDialog::uploadFinished()
{
    //plog(tr("Complete: %1").arg(currentReply->readAll()));
    qDebug() << "Complete: " + currentReply->readAll();
}

void CopyPhotosDialog::uploadError()
{
    qDebug() << "Error: " + currentReply->errorString();
}
