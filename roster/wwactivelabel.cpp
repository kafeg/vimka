#include "wwactivelabel.h"
#include <QMouseEvent>

wwActiveLabel::wwActiveLabel(QWidget *parent) : QLabel(parent) {
    m_pressed = false;
    setText("wwActiveLabel");
}
wwActiveLabel::~wwActiveLabel() {}
void wwActiveLabel::mouseDoubleClickEvent( QMouseEvent * e ) {
    if(e->button() == Qt::LeftButton)
        emit doubleClicked();
    QLabel::mouseDoubleClickEvent(e);
}
void wwActiveLabel::mousePressEvent( QMouseEvent * e ) {
    if(e->button() == Qt::LeftButton) {
        m_pressed = true;
    } else
        m_pressed = false;
    emit pressed();
}
void wwActiveLabel::mouseReleaseEvent( QMouseEvent * e ) {
    if(m_pressed && e->button() == Qt::LeftButton && hitLabel(e->pos()))
        emit clicked();
    m_pressed = false;
    emit released();

}
void wwActiveLabel::enterEvent( QEvent *e ) {
    QLabel::enterEvent(e);
    emit mouseOver();
}
void wwActiveLabel::leaveEvent( QEvent *e ) {
    QLabel::leaveEvent(e);
    emit mouseOut();
}
bool wwActiveLabel::hitLabel( const QPoint & p ){
    return rect().contains(p);
}

void wwActiveLabel::setText(const QString &text)
{
    QLabel::setText(text);
    setToolTip(text + tr("\nDouble Click for edit your activity."));
}
