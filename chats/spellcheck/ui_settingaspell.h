/********************************************************************************
** Form generated from reading ui file 'settingaspell.ui'
**
** Created: Sun 9. Nov 11:42:27 2008
**      by: Qt User Interface Compiler version 4.4.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_SETTINGASPELL_H
#define UI_SETTINGASPELL_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QSpacerItem>

QT_BEGIN_NAMESPACE

class Ui_settingAspellClass
{
public:
    QGridLayout *gridLayout;
    QLabel *label;
    QLineEdit *aspellPath;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *settingAspellClass)
    {
    if (settingAspellClass->objectName().isEmpty())
        settingAspellClass->setObjectName(QString::fromUtf8("settingAspellClass"));
    settingAspellClass->resize(400, 97);
    gridLayout = new QGridLayout(settingAspellClass);
    gridLayout->setSpacing(6);
    gridLayout->setMargin(11);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    label = new QLabel(settingAspellClass);
    label->setObjectName(QString::fromUtf8("label"));

    gridLayout->addWidget(label, 0, 0, 1, 1);

    aspellPath = new QLineEdit(settingAspellClass);
    aspellPath->setObjectName(QString::fromUtf8("aspellPath"));

    gridLayout->addWidget(aspellPath, 0, 1, 1, 1);

    hboxLayout = new QHBoxLayout();
    hboxLayout->setSpacing(6);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout->addItem(spacerItem);

    buttonBox = new QDialogButtonBox(settingAspellClass);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    hboxLayout->addWidget(buttonBox);


    gridLayout->addLayout(hboxLayout, 1, 0, 1, 2);


    retranslateUi(settingAspellClass);
    QObject::connect(buttonBox, SIGNAL(accepted()), settingAspellClass, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), settingAspellClass, SLOT(reject()));

    QMetaObject::connectSlotsByName(settingAspellClass);
    } // setupUi

    void retranslateUi(QDialog *settingAspellClass)
    {
    settingAspellClass->setWindowTitle(QApplication::translate("settingAspellClass", "settingAspell", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("settingAspellClass", "Dictionary:", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(settingAspellClass);
    } // retranslateUi

};

namespace Ui {
    class settingAspellClass: public Ui_settingAspellClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGASPELL_H
