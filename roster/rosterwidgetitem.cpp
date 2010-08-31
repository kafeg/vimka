#include "rosterwidgetitem.h"

RosterWidgetItem::RosterWidgetItem(QString text) :
        QObject(), QStandardItem()
{
    setText(text);
}

