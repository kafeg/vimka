#ifndef EMOTICONS_H
#define EMOTICONS_H

#include <QObject>
#include <QMap>
#include "emoticons/movielabel.h"

class VKEngine;

class Emoticons : public QObject
{
    Q_OBJECT
public:
    explicit Emoticons(VKEngine *parent);

    QString replaceEmoticons(QString message);

    QMap<QString, movieLabel *> allEmotiocnsWidgets;

signals:

public slots:

private:
   // QString themeName;
    QMap<QString, QString> allEmotiocns;
    VKEngine *engine;

};

#endif // EMOTICONS_H
