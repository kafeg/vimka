/*
    emoticonMenu

    Copyright (c) 2008 by Rustam Chakin <qutim.develop@gmail.com>
                                  2009 by Ruslan Nigmatullin <euroelessar@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/


#ifndef CHATEMOTICONMENU_H
#define CHATEMOTICONMENU_H

#include <QtGui>
#include <cmath>
#include "movielabel.h"
#include "../emoticons.h"

class ChatEmoticonMenu : public QScrollArea
{
    Q_OBJECT

public:
    ChatEmoticonMenu(QWidget *parent, Emoticons *emots);
    ~ChatEmoticonMenu();
public slots:

signals:
    void insertSmile(const QString &);

private:
    QList<movieLabel *> labelList;
    QGridLayout *m_grid_layout;
    QWidget *m_widget;
    Emoticons *emoticons;
};

#endif // CHATEMOTICONMENU_H
