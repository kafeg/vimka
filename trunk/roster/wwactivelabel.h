#ifndef WWCLASSES_H
#define WWCLASSES_H

#include <QLabel>
#include <QTimer>
#include <QPoint>

class wwActiveLabel : public QLabel
{
    Q_OBJECT
public:
    wwActiveLabel(QWidget *parent = 0); ///< default constructor
    ~wwActiveLabel();
    void setText(const QString &text);
signals:
    void doubleClicked(); ///< double click with LMB
    void clicked(); ///< single click with LMB
    void pressed(); ///< LMB pressed
    void released(); ///< LMB released
    void mouseOver(); ///< mouse pointer just entered the widget
    void mouseOut(); ///< mouse pointer just left the widget
protected:
    void mouseDoubleClickEvent ( QMouseEvent * e );
    void mousePressEvent( QMouseEvent *e);
    void mouseReleaseEvent( QMouseEvent *e);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    bool hitLabel(const QPoint &p);
    bool m_pressed;
};

#endif
