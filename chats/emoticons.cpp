#include "emoticons.h"
#include <QFile>
#include <QDomNode>
#include <QDomNodeList>
#include <QDomElement>
#include <QDebug>
#include "../vk/vkengine.h"
#include "../vimkamain.h"
#include "../settingsmanager.h"

Emoticons::Emoticons(VKEngine *parent) :
        QObject(parent)
{
    //themeName = "Oxygen";
    engine = parent;


    QFile file(engine->m_rosterWindow->settingsMngr->styleDir()+"/emoticons/emoticons.xml");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QString xml = file.readAll();

    QDomDocument doc("Emoticons");
    doc.setContent(xml);
    QDomElement docElem = doc.documentElement();
    QDomNodeList emots = docElem.elementsByTagName ( "emoticon" );
    for(int i=0; i < emots.count(); i++){
        QDomElement emotElem = emots.at(i).toElement();
        QDomNodeList strs = emotElem.elementsByTagName ( "string" );
        QString emotFileName = engine->m_rosterWindow->settingsMngr->styleDir()+"/emoticons/"
                               + emotElem.attribute("file") + ".png";
        QFile f(emotFileName);
        if (!f.exists()){
            continue;
        }

        QString emotHtml = "<img title=\""+QRegExp::escape(strs.at(0).toElement().text())+"\" src=\"file:///"+emotFileName+"\" />";
        //qDebug() << emotHtml;
        for(int i=0; i < strs.count(); i++){
            QDomElement strElem = strs.at(i).toElement();
            allEmotiocns.insert(strElem.text(), emotHtml);
            //qDebug() << strElem.text() << emotHtml;
        }
        movieLabel *emotLbl = new movieLabel();
        emotLbl->setPixmap(QPixmap::fromImage(QImage(emotFileName)));
        emotLbl->setToolTip(strs.at(0).toElement().text());
        allEmotiocnsWidgets.insert(strs.at(0).toElement().text(), emotLbl);
    }
}

QString Emoticons::replaceEmoticons(QString message)
{
    QString mess = message;
    QMapIterator<QString, QString> i(allEmotiocns);
    while(i.hasNext()){
        i.next();
        mess = mess.replace(i.key(), i.value());
    }

    //qDebug() << mess;
    return mess;
}
