#include "wwactivelineedit.h"

wwActiveLineEdit::wwActiveLineEdit(QWidget *parent) :
    QLineEdit(parent)
{
}

void wwActiveLineEdit::keyPressEvent ( QKeyEvent * event )
{
    if (event->key() == Qt::Key_Escape){
        emit escapePressed();
    }
    QLineEdit::keyPressEvent(event);
}
