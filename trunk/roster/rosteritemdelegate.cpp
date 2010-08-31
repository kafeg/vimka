#include "rosteritemdelegate.h"
#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPainter>
#include <QFile>

RosterItemDelegate::RosterItemDelegate(QWidget *parent) :
        QStyledItemDelegate(parent)
{
    container = new QWidget();
    container->setAttribute(Qt::WA_NoSystemBackground);
    container->setObjectName("RosterItemDelegate");
    //container->setMinimumHeight(40);
    //container->setSizeHint(QSize(100,50));
    //setAutoFillBackground(true);

    QHBoxLayout *layoutH = new QHBoxLayout(container);
    icon = new QLabel();
    container->layout()->addWidget(icon);

    QWidget *containerV = new QWidget();

    QVBoxLayout *layoutV = new QVBoxLayout(containerV);
    container->layout()->addWidget(containerV);

    name = new QLabel("username");
    status = new QLabel("status");

    containerV->layout()->addWidget(name);
    containerV->layout()->addWidget(status);

    container->layout()->setSpacing(0);
    containerV->layout()->setSpacing(0);
    layoutH->addStretch();
    layoutH->setMargin(1);
    layoutV->addStretch();
    layoutV->setMargin(1);

    QFile file(":/styles/vkontakte.css");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    style = file.readAll();

    container->setStyleSheet(style);

    //qApp->setStyleSheet(vkStyle);
}

void RosterItemDelegate::paint(QPainter *painter,
                               const QStyleOptionViewItem &option,
                               const QModelIndex &index) const
{
    if(option.state & QStyle::State_HasFocus)
    {
        painter->save();
        QPen pen = QPen(Qt::green);
        pen.setWidth(2);
        painter->setPen(pen);
        painter->fillRect(option.rect, Qt::SolidPattern);
        painter->restore();
    }

    container->resize(option.rect.size());
    painter->save();
    painter->translate(option.rect.topLeft());
    container->render(painter);
    painter->restore();

}
