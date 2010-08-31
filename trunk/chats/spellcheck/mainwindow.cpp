/****************************************************************************
 **
 ** Copyright (C) 2005-2008 Trolltech ASA. All rights reserved.
 **
 ** This file is part of the example classes of the Qt Toolkit.
 **
 ** This file may be used under the terms of the GNU General Public
 ** License versions 2.0 or 3.0 as published by the Free Software
 ** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
 ** included in the packaging of this file.  Alternatively you may (at
 ** your option) use any later version of the GNU General Public
 ** License if such license has been publicly approved by Trolltech ASA
 ** (or its successors, if any) and the KDE Free Qt Foundation. In
 ** addition, as a special exception, Trolltech gives you certain
 ** additional rights. These rights are described in the Trolltech GPL
 ** Exception version 1.1, which can be found at
 ** http://www.trolltech.com/products/qt/gplexception/ and in the file
 ** GPL_EXCEPTION.txt in this package.
 **
 ** Please review the following information to ensure GNU General
 ** Public Licensing requirements will be met:
 ** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
 ** you are unsure which license is appropriate for your use, please
 ** review the following information:
 ** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
 ** or contact the sales department at sales@trolltech.com.
 **
 ** In addition, as a special exception, Trolltech, as the sole
 ** copyright holder for Qt Designer, grants users of the Qt/Eclipse
 ** Integration plug-in the right for the Qt/Eclipse Integration to
 ** link to functionality provided by Qt Designer and its related
 ** libraries.
 **
 ** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
 ** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
 ** granted herein.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ****************************************************************************/

#include <QtGui>
#include <QTextCursor>
#include <iostream>

#include "mainwindow.h"
#include "settingaspell.h"
#include "SpellTextEdit.h"

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent) {
        setupFileMenu();
        setupSettingMenu();
        setupHelpMenu();
        SpellDic=QApplication::applicationDirPath() + "/ru_RU.dic";
        setupEditor();

        setCentralWidget(editor);
        setWindowTitle(tr("Spell Checker"));

}

void MainWindow::about() {
        QMessageBox::about(this, tr("About Online Spellchecker"),
                        tr("<p>The <b>online spell checker</b> shows how "
                                "to perform simple online spell checking</p>"));
}

void MainWindow::newFile() {
        editor->clear();
}

void MainWindow::setSpell() {
        QString fileName = QFileDialog::getOpenFileName(this,
               tr("Select Dictionary "), SpellDic, tr("Dictionary (*.dic)"));

        QString new_Dict=fileName;
        if (SpellDic != new_Dict)
        {
                SpellDic=new_Dict;
                highlighter->setDict(SpellDic);
                editor->setDict(SpellDic);
        }
}

void MainWindow::debug() {
        //
        ;
}

void MainWindow::openFile(const QString &path) {
        QString fileName = path;

        if (fileName.isNull())
                fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "",
                                "C++ Files (*.cpp *.h);;Text (*.txt);;LaTeX (*.tex)");

        if (!fileName.isEmpty()) {
                QFile file(fileName);
                if (file.open(QFile::ReadOnly | QFile::Text)) {
                        editor->setPlainText(file.readAll());
                }
        }
}

void MainWindow::setupEditor() {
        QFont font;
        font.setFamily("Courier");
        font.setFixedPitch(true);
        font.setPointSize(10);

        editor = new SpellTextEdit;
        editor->setFont(font);
        editor->setDict(SpellDic);

        highlighter = new Highlighter(editor->document(),SpellDic,true);
        connect(editor,SIGNAL(addWord(QString)),highlighter,SLOT(slot_addWord(QString)));

        QFile file("mainwindow.h");
        if (file.open(QFile::ReadOnly | QFile::Text))
                editor->setPlainText(file.readAll());
}

void MainWindow::setupFileMenu() {
        QMenu *fileMenu = new QMenu(tr("&File"), this);
        menuBar()->addMenu(fileMenu);

        fileMenu->addAction(tr("&New"), this, SLOT(newFile()), QKeySequence(tr("Ctrl+N", "File|New")));
        fileMenu->addAction(tr("&Open..."), this, SLOT(openFile()), QKeySequence(tr("Ctrl+O", "File|Open")));
        fileMenu->addAction(tr("E&xit"), qApp, SLOT(quit()), QKeySequence(tr("Ctrl+Q", "File|Exit")));
}

void MainWindow::setupHelpMenu() {
        QMenu *helpMenu = new QMenu(tr("&Help"), this);
        menuBar()->addMenu(helpMenu);

        helpMenu->addAction(tr("&About"), this, SLOT(about()));
        helpMenu->addAction(tr("&Debug"), this, SLOT(debug()));
        helpMenu->addAction(tr("About &Qt"), qApp, SLOT(aboutQt()));
}

void MainWindow::setupSettingMenu() {
        QMenu *settingMenu = new QMenu(tr("&Settings"), this);
        menuBar()->addMenu(settingMenu);

        settingMenu->addAction(tr("&ASpell Settings"), this, SLOT(setSpell()));
}

