#ifndef PHOTOVIEWER_H
#define PHOTOVIEWER_H

#include <QObject>
#include <QUrl>
#include <QDeclarativePropertyMap>

class QDeclarativeView;

class PhotoViewer : public QObject
{
    Q_OBJECT
public:
    explicit PhotoViewer(QDeclarativeView *view, QObject *parent = 0);

    void showPhoto(QUrl photo);

signals:

public slots:

private:
    QDeclarativeView *m_view;
    QDeclarativePropertyMap ownerData;

};

#endif // PHOTOVIEWER_H
