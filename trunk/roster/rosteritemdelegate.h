#ifndef ROSTERITEMDELEGATE_H
#define ROSTERITEMDELEGATE_H

#include <QStyledItemDelegate>

class QLabel;

class RosterItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit RosterItemDelegate(QWidget *parent = 0);

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;

signals:

public slots:

private:
    QWidget *container;
    int online;
    QLabel *name;
    QLabel *status;
    QLabel *icon;
    QString style;

};

#endif // ROSTERITEMDELEGATE_H
