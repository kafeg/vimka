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


#include "chatemoticonmenu.h"
#include <QVector>

ChatEmoticonMenu::ChatEmoticonMenu(QWidget *parent, Emoticons *emots)
    : QScrollArea(parent)
{
    m_widget = 0;
    m_grid_layout = 0;
    setFrameStyle(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_widget = new QWidget;
    m_grid_layout = new QGridLayout(m_widget);
    m_grid_layout->setSpacing(1);
    m_widget->setLayout(m_grid_layout);

    emoticons = emots;

    QMapIterator<QString, movieLabel *> i(emoticons->allEmotiocnsWidgets);
    int rowCount = 5, curRow = 0, curCol = -1;
    while (i.hasNext()){
        curCol++;
        i.next();
        m_grid_layout->addWidget(i.value(),curRow,curCol);

        if (curCol == rowCount){
            curRow++;
            curCol = -1;
        }

        connect(i.value(), SIGNAL(sendMovieTip(const QString &)), this, SIGNAL(insertSmile(const QString &)));
    }

    setWidget(m_widget);
}

ChatEmoticonMenu::~ChatEmoticonMenu()
{
    m_widget = 0;
}
