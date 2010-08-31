HEADERS = hunspell/affentry.hxx \
    hunspell/affixmgr.hxx \
    hunspell/atypes.hxx \
    hunspell/baseaffix.hxx \
    hunspell/csutil.hxx \
    hunspell/dictmgr.hxx \
    hunspell/filemgr.hxx \
    hunspell/hashmgr.hxx \
    hunspell/htypes.hxx \
    hunspell/hunspell.h \
    hunspell/hunspell.hxx \
    hunspell/hunzip.hxx \
    hunspell/langnum.hxx \
    hunspell/phonet.hxx \
    hunspell/suggestmgr.hxx \
    hunspell/w_char.hxx \
    settingaspell.h \
    SpellTextEdit.h \
    highlighter.h \
    mainwindow.h
SOURCES = hunspell/affentry.cxx \
    hunspell/affixmgr.cxx \
    hunspell/csutil.cxx \
    hunspell/dictmgr.cxx \
    hunspell/filemgr.cxx \
    hunspell/hashmgr.cxx \
    hunspell/hunspell.cxx \
    hunspell/hunzip.cxx \
    hunspell/phonet.cxx \
    hunspell/suggestmgr.cxx \
    hunspell/utf_info.cxx \
    settingaspell.cpp \
    SpellTextEdit.cpp \
    highlighter.cpp \
    mainwindow.cpp \
    main.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/richtext/syntaxhighlighter
sources.files = $$SOURCES \
    $$HEADERS \
    $$RESOURCES \
    $$FORMS \
    syntaxhighlighter.pro \
    examples
sources.path = $$[QT_INSTALL_EXAMPLES]/richtext/syntaxhighlighter
INSTALLS += target \
    sources
FORMS = settingaspell.ui
RESOURCES += 
