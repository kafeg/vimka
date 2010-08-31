#include "chattextedit.h"
#include <QKeyEvent>
#include <QLocale>
#include "spellcheck/highlighter.h"
#include "../vimkamain.h"
#include "../settingsmanager.h"

ChatTextEdit::ChatTextEdit(VimkaMain *rosterWindow, QWidget *parent) :
    SpellTextEdit(parent)
{
    m_rosterWindow = rosterWindow;

    //проверка орфографии
    if ( QLocale::system().language () == QLocale::Russian ) {
        //SpellDic = ":/spellcheck/ru_RU.dic";
        SpellDic = m_rosterWindow->settingsMngr->dataDir() + "/dic/ru_RU.dic";
    }

    if ( QLocale::system().language () == QLocale::English ) {
        //SpellDic = ":/spellcheck/en_GB.dic";
        SpellDic = m_rosterWindow->settingsMngr->dataDir() + "/dic/en_GB.dic";
    }

    setDict(SpellDic);

    highlighter = new Highlighter(document(),SpellDic,true);
    connect(this,SIGNAL(addWord(QString)),highlighter,SLOT(slot_addWord(QString)));
}

void ChatTextEdit::keyPressEvent(QKeyEvent *e)
{
    switch(e->key()){
    case Qt::Key_Enter:
    case Qt::Key_Return:
        if (!(e->modifiers() & Qt::ControlModifier)){
            emit returnPressed();
        }else{
            e->setModifiers(Qt::NoModifier);
            SpellTextEdit::keyPressEvent(e);
        }
        break;
    default:
        SpellTextEdit::keyPressEvent(e);
    }
}

void ChatTextEdit::focusInEvent(QFocusEvent *e)
{
    emit focusInChatEdit();
    SpellTextEdit::focusInEvent( e );
}
