# -------------------------------------------------
# Project created by QtCreator 2010-07-27T19:40:03
# -------------------------------------------------
QT += core \
    gui \
    xml \
    network \
    webkit \
    phonon \
    declarative

DEFINES += MOBILE_UI

#android {
#QT -= phonon
#}

#DESTDIR = bin
#OBJECTS_DIR = build
#MOC_DIR = build
#UI_DIR = build

TARGET = vimka
TEMPLATE = app

SOURCES += main.cpp \
    vimkamain.cpp \
    roster/rosterwidgetitem.cpp \
    chats/chats.cpp \
    chats/personalchat.cpp \
    chats/chattextedit.cpp \
    trayicon.cpp \
    vk/vkengine.cpp \
    vk/vkrequest.cpp \
    vk/vkroster.cpp \
    settingsmanager.cpp \
    roster/qhttpimage.cpp \
    roster/wwactivelabel.cpp \
    roster/wwactivelineedit.cpp \
    vk/vkmessage.cpp \
    chats/messagestyler.cpp \
    chats/declarative/albumsmodel.cpp \
    chats/spellcheck/SpellTextEdit.cpp \
    chats/spellcheck/highlighter.cpp \
    chats/spellcheck/hunspell/phonet.cxx \
    chats/spellcheck/hunspell/hunzip.cxx \
    chats/spellcheck/hunspell/utf_info.cxx \
    chats/spellcheck/hunspell/suggestmgr.cxx \
    chats/spellcheck/hunspell/hunspell.cxx \
    chats/spellcheck/hunspell/hashmgr.cxx \
    chats/spellcheck/hunspell/filemgr.cxx \
    chats/spellcheck/hunspell/dictmgr.cxx \
    chats/spellcheck/hunspell/csutil.cxx \
    chats/spellcheck/hunspell/affixmgr.cxx \
    chats/spellcheck/hunspell/affentry.cxx \
    chats/emoticons.cpp \
    roster/rosteritemdelegate.cpp \
    chats/videolistmodel.cpp \
    chats/emoticons/movielabel.cpp \
    chats/emoticons/chatemoticonmenu.cpp \
    roster/rosterproxymodel.cpp \
    chats/declarative/photoviewer.cpp \
    aboutdialog.cpp \
    SlidingStackedWidget.cpp \
    copyphotosdialog.cpp

!android {
    SOURCES += chats/vkmediafactory.cpp \
    chats/vkmediaview.cpp \
    chats/mediaplayer.cpp \
}

HEADERS += vimkamain.h \
    roster/rosterwidgetitem.h \
    chats/chats.h \
    chats/personalchat.h \
    chats/chattextedit.h \
    trayicon.h \
    vk/vkengine.h \
    vk/vkrequest.h \
    vk/vkroster.h \
    settingsmanager.h \
    roster/qhttpimage.h \
    roster/wwactivelabel.h \
    roster/wwactivelineedit.h \
    vk/vkmessage.h \
    chats/messagestyler.h \
    chats/declarative/albumsmodel.h \
    chats/spellcheck/SpellTextEdit.h \
    chats/spellcheck/highlighter.h \
    chats/spellcheck/hunspell/langnum.hxx \
    chats/spellcheck/hunspell/hunzip.hxx \
    chats/spellcheck/hunspell/hunspell.hxx \
    chats/spellcheck/hunspell/hunspell.h \
    chats/spellcheck/hunspell/w_char.hxx \
    chats/spellcheck/hunspell/suggestmgr.hxx \
    chats/spellcheck/hunspell/phonet.hxx \
    chats/spellcheck/hunspell/htypes.hxx \
    chats/spellcheck/hunspell/hashmgr.hxx \
    chats/spellcheck/hunspell/filemgr.hxx \
    chats/spellcheck/hunspell/dictmgr.hxx \
    chats/spellcheck/hunspell/csutil.hxx \
    chats/spellcheck/hunspell/baseaffix.hxx \
    chats/spellcheck/hunspell/atypes.hxx \
    chats/spellcheck/hunspell/affixmgr.hxx \
    chats/spellcheck/hunspell/affentry.hxx \
    chats/emoticons.h \
    roster/rosteritemdelegate.h \
    chats/videolistmodel.h \
    chats/emoticons/movielabel.h \
    chats/emoticons/chatemoticonmenu.h \
    roster/rosterproxymodel.h \
    chats/declarative/photoviewer.h \
    aboutdialog.h \
    SlidingStackedWidget.h \
    copyphotosdialog.h

!android {
HEADERS += chats/vkmediafactory.h \
    chats/vkmediaview.h \
    chats/mediaplayer.h \
}

FORMS += vimkamain.ui \
    chats/chats.ui \
    chats/personalchat.ui \
    aboutdialog.ui \
    copyphotosdialog.ui
RESOURCES += resources/vimka.qrc

RC_FILE = resources/vimka.rc

unix {

target.path =  ../vimka-build-desktop/debian/vimka/usr/bin

images.path = ../vimka-build-desktop/debian/vimka/usr/share/icons/hicolor/
images.files = resources/hicolor/*

desktops.path =  ../vimka-build-desktop/debian/vimka/usr/share/applications/
desktops.files = resources/vimka.desktop

data.path =  ../vimka-build-desktop/debian/vimka/usr/share/vimka/data/
data.files = resources/data/*

}

unix:INSTALLS += target
unix:INSTALLS += images
unix:INSTALLS += desktops
unix:INSTALLS += data

OTHER_FILES += \
    android/AndroidManifest.xml \
    android/src/eu/licentia/necessitas/industrius/QtActivity.java \
    android/src/eu/licentia/necessitas/industrius/QtSurface.java \
    android/src/eu/licentia/necessitas/industrius/QtApplication.java \
    android/src/eu/licentia/necessitas/ministro/IMinistro.aidl \
    android/src/eu/licentia/necessitas/ministro/IMinistroCallback.aidl \
    android/res/drawable-mdpi/icon.png \
    android/res/values/strings.xml \
    android/res/values/libs.xml \
    android/res/drawable-ldpi/icon.png \
    android/res/drawable-hdpi/icon.png \
    android/AndroidManifest.xml \
    android/src/eu/licentia/necessitas/industrius/QtActivity.java \
    android/src/eu/licentia/necessitas/industrius/QtSurface.java \
    android/src/eu/licentia/necessitas/industrius/QtApplication.java \
    android/src/eu/licentia/necessitas/ministro/IMinistro.aidl \
    android/src/eu/licentia/necessitas/ministro/IMinistroCallback.aidl \
    android/res/drawable-mdpi/icon.png \
    android/res/values/strings.xml \
    android/res/values/libs.xml \
    android/res/drawable-ldpi/icon.png \
    android/res/drawable-hdpi/icon.png



