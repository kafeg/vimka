#include <QtGui/QApplication>
#include "vimkamain.h"
#include <QTranslator>
#include <QLibraryInfo>
#include <QTextCodec>
#include <QLocale>
#include <QWebSettings>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Q_INIT_RESOURCE(vimka);

    QTextCodec* codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForCStrings(codec);

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
                      QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&qtTranslator);

    QTranslator myappTranslator;
    myappTranslator.load(":/translates/locale_" + QLocale::system().name()+".qm");
    a.installTranslator(&myappTranslator);

    /*if ( QLocale::system().language () == QLocale::Russian ) {

        myappTranslator.load(":/translates/locale_ru_RU.qm" );
    }*/

    //qDebug() << QLocale::system().name();

    QCoreApplication::setApplicationName("Vimka");
    QCoreApplication::setApplicationVersion("0.4.3");
    QCoreApplication::setOrganizationName("Petrov Vitaly (Kafeg) <v31337@gmail.com>");
    QCoreApplication::setOrganizationDomain("http://vkontakte.ru/club19099587");

    QWebSettings::globalSettings()->setAttribute(
            QWebSettings::PluginsEnabled, true);

    VimkaMain w;
    w.show();

    return a.exec();
}
