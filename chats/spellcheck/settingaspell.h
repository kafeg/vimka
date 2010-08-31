#ifndef SETTINGASPELL_H
#define SETTINGASPELL_H

#include <QtGui/QDialog>
#include "ui_settingaspell.h"

class settingAspell : public QDialog
{
    Q_OBJECT

public:
    settingAspell(QWidget *parent = 0);
    ~settingAspell();
    void setPath(const QString &text);
    QString getPath();

private:
    Ui::settingAspellClass ui;
};

#endif // SETTINGASPELL_H
