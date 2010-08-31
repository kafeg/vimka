#ifndef ROSTERWIDGETITEM_H
#define ROSTERWIDGETITEM_H

#include <QObject>
#include <QStandardItem>

class RosterWidgetItem : public QObject, public QStandardItem
{
    Q_OBJECT
public:
    explicit RosterWidgetItem(QString text/*, QString settingsDir*/);

signals:

public slots:
    void applyIcon(QIcon icon) { setIcon(icon); }
};

#endif // ROSTERWIDGETITEM_H
