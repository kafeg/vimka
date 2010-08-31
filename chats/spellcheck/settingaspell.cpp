#include "settingaspell.h"

settingAspell::settingAspell(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);
}

settingAspell::~settingAspell()
{

}

void settingAspell::setPath(const QString &text)
{
	ui.aspellPath->setText(text);
}

QString settingAspell::getPath()
{
	return ui.aspellPath->text();
}
