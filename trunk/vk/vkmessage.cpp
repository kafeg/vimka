#include "vkmessage.h"
#include "vkengine.h"
#include <QDebug>

VKMessage::VKMessage(VKEngine *engine) :
    QObject(engine)
{
    m_deleted = false;
    fromHistory = false;
    vkEngine = engine;
    isFirst = false;

    styledText = "";

    connect(vkEngine, SIGNAL(lps0DeleteMessage(QString)),
            this,SLOT(fromlps0DeleteMessage(QString)));

    connect(vkEngine, SIGNAL(lps1ReplaceMessageFlags(QString,int)),
            this,SLOT(fromlps1ReplaceMessageFlags(QString,int)));

    connect(vkEngine, SIGNAL(lps2SetMessageFlags(QString,int)),
            this,SLOT(fromlps2SetMessageFlags(QString,int)));

    connect(vkEngine, SIGNAL(lps3ResetMessageFlags(QString,int)),
            this,SLOT(fromlps3ResetMessageFlags(QString,int)));
}

void VKMessage::setFlags(int flags)
{
    this->flags = flags;
}

//todo test
void  VKMessage::fromlps0DeleteMessage(QString local_id)
{
    if (this->local_id == local_id){
        m_deleted = true;
        emit changed(this);
    }
    //qDebug() << local_id << " deleted";
}

//todo test
void  VKMessage::fromlps1ReplaceMessageFlags(QString local_id, int flags)
{
    if (this->local_id == local_id){
        this->flags = flags;
        emit changed(this);
    }
    //qDebug() << local_id << " replaced " << flags;
}

//todo test
void  VKMessage::fromlps2SetMessageFlags(QString local_id, int mask)
{
    if (this->local_id == local_id){
        this->flags |= mask;
        emit changed(this);
    }
    //qDebug() << local_id << " set " << flags;
}

//todo test
void  VKMessage::fromlps3ResetMessageFlags(QString local_id, int mask)
{
    if (this->local_id == local_id){
        this->flags &=~ mask;
        emit changed(this);
    }
    //qDebug() << local_id << " reset " << flags;
}
