#ifndef CHATTEXTEDIT_H
#define CHATTEXTEDIT_H

#include "spellcheck/SpellTextEdit.h"

class Highlighter;
class VimkaMain;

class ChatTextEdit : public SpellTextEdit
{
    Q_OBJECT
public:
    explicit ChatTextEdit(VimkaMain *rosterWindow, QWidget *parent = 0);

    void keyPressEvent(QKeyEvent *e);
    void focusInEvent(QFocusEvent *e);

    //проверка орфографии
    Highlighter *highlighter;
    QString SpellDic;

signals:
    void returnPressed();
    void focusInChatEdit();

public slots:

private:
    VimkaMain *m_rosterWindow;

};

#endif // CHATTEXTEDIT_H
