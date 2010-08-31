#ifndef WWACTIVELINEEDIT_H
#define WWACTIVELINEEDIT_H

#include <QLineEdit>
#include <QKeyEvent>

class wwActiveLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit wwActiveLineEdit(QWidget *parent = 0);

signals:
    void escapePressed();

protected:
    void keyPressEvent ( QKeyEvent * event );

public slots:

};

#endif // WWACTIVELINEEDIT_H
