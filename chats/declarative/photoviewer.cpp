#include "photoviewer.h"
#include <QDeclarativeView>
#include <QDeclarativeContext>

PhotoViewer::PhotoViewer(QDeclarativeView *view, QObject *parent) :
    QObject(parent)
{
    m_view = view;
    ownerData.insert("photo", QVariant(QString("")));
    QDeclarativeContext *ctxt = m_view->rootContext();
    ctxt->setContextProperty("owner", &ownerData);

    m_view->setSource(QUrl("qrc:/declarative/photo/photo.qml"));
}

void PhotoViewer::showPhoto(QUrl photo)
{
    ownerData.insert("photo", QVariant(photo.toString()));
}
