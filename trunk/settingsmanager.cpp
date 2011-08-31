#include "settingsmanager.h"
#include "vimkamain.h"
#include "ui_vimkamain.h"
#include <QSettings>
#include <QDir>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QFile>
#include <QTextStream>
#include "chats/chats.h"
#include <QNetworkProxy>
#include "SlidingStackedWidget.h"

SettingsManager::SettingsManager(VimkaMain *rosterWindows,
                                 QObject *parent) :
    QObject(parent)
{
    m_rosterWindows = rosterWindows;
    currentStyle = "vk";

    QDir dr(dataDir());
    if (!dr.exists()){
        dr.mkpath(dataDir());
    }

    loadStyle();

    //m_vkEngine = engine;
}

void SettingsManager::loadSettings()
{
    m_rosterWindows->slidingStacked->slideInIdx(m_rosterWindows->slidingStacked->indexOf(m_rosterWindows->ui->loginPage));

    /*QSettings settings(settingsDir()+"/vimka"+
                       QApplication::applicationVersion()
                       +".ini", QSettings::IniFormat);*/
    QSettings settings(settingsDir()+"/vimka.ini", QSettings::IniFormat);

    settings.beginGroup("Profiles");

    m_rosterWindows->emails = settings.value("emails").toStringList();
    m_rosterWindows->lastPass = settings.value("lastPass").toString();
    m_rosterWindows->lastEmail = settings.value("lastEmail").toString();
    m_rosterWindows->ui->cbAutoLogin->setChecked( settings.value("autoLogin").toBool());
    settings.endGroup();

    QRect rect = QApplication::desktop()->geometry();
    QPoint defPos = rect.center() - m_rosterWindows->rect().center();

    settings.beginGroup("MainWindow");
    m_rosterWindows->resize(settings.value("size", QSize(240, 440)).toSize());
    m_rosterWindows->move(settings.value("pos", defPos).toPoint());
    m_rosterWindows->ui->tbStatus->setChecked(settings.value("actionOnlyOnlineIsTriggered", false).toBool());
    m_rosterWindows->ui->tbSound->setChecked(settings.value("tbSoundIsChecked", true).toBool());
    settings.endGroup();

    settings.beginGroup("ChatsWindow");
    m_rosterWindows->chats->resize(settings.value("size", QSize(400, 350)).toSize());
    m_rosterWindows->chats->move(settings.value("pos", defPos).toPoint());
    settings.endGroup();

    m_rosterWindows->ui->cbMails->addItems(m_rosterWindows->emails);

    for (int i = 0; i < m_rosterWindows->ui->cbMails->count(); i++){
        if (m_rosterWindows->ui->cbMails->itemText(i) == m_rosterWindows->lastEmail) {
            m_rosterWindows->ui->cbMails->setCurrentIndex(i);
            m_rosterWindows->ui->lePass->setText(m_rosterWindows->lastPass);
            break;
        }
    }

    settings.beginGroup("Proxy");

    m_rosterWindows->ui->gbProxy->setChecked(settings.value("useProxy", false).toBool());
    m_rosterWindows->ui->gbProxyAuth->setChecked(settings.value("useProxyAuth", false).toBool());
    m_rosterWindows->ui->cbProxyType->setCurrentIndex(settings.value("proxyType", 0).toInt());
    m_rosterWindows->ui->leProxyHost->setText(settings.value("proxyHost", "").toString());
    m_rosterWindows->ui->sbProxyPort->setValue(settings.value("proxyPort", 5000).toInt());

    if (m_rosterWindows->ui->gbProxyAuth->isChecked()){
        m_rosterWindows->ui->leProxyLogin->setText(settings.value("proxyLogin", "").toString());
        m_rosterWindows->ui->leProxyPass->setText(settings.value("proxyPassword", "").toString());
    }

    settings.endGroup();

    applySettings();

}

void SettingsManager::saveSettings()
{
    QDir settingsDirChecker(settingsDir());
    if (!settingsDirChecker.exists()){
        settingsDirChecker.mkpath(settingsDir());
    }

    QSettings settings(settingsDir()+"/vimka.ini", QSettings::IniFormat);

    settings.beginGroup("Profiles");

    settings.setValue("emails",m_rosterWindows->emails );
    settings.setValue("lastEmail", m_rosterWindows->lastEmail);
    settings.setValue("lastPass", m_rosterWindows->lastPass);
    if (m_rosterWindows->ui->cbAutoLogin->isChecked()){
        settings.setValue("autoLogin", true);
    }else{
        settings.setValue("autoLogin", false);
    }

    settings.endGroup();

    settings.beginGroup("MainWindow");
    settings.setValue("size", m_rosterWindows->size());
    settings.setValue("pos", m_rosterWindows->pos());
    settings.setValue("actionOnlyOnlineIsTriggered",
                      m_rosterWindows->ui->tbStatus->isChecked());
    settings.setValue("tbSoundIsChecked",
                      m_rosterWindows->ui->tbSound->isChecked());
    settings.endGroup();

    settings.beginGroup("ChatsWindow");
    settings.setValue("size", m_rosterWindows->chats->size());
    settings.setValue("pos", m_rosterWindows->chats->pos());
    settings.endGroup();

}

QString SettingsManager::settingsDir()
{
    //return QApplication::applicationDirPath()+"/settings/";

    return QDesktopServices::storageLocation(QDesktopServices::HomeLocation)+"/.vimka/";
}

QString SettingsManager::dataDir()
{
    //qDebug() << QApplication::applicationDirPath();
    if (QApplication::applicationDirPath() == "/usr/bin"){
        return "/usr/share/vimka/data/";
    }else{
        return QApplication::applicationDirPath() + "/data/";
    }
}

QString SettingsManager::styleDir()
{
    return stylesDir() + currentStyle;
}

QString SettingsManager::stylesDir()
{
    return dataDir() + "styles/";
}

void SettingsManager::loadStyle()
{

    //РіСЂСѓР·РёРј СЃС‚РёР»СЊ
    QFile file(styleDir()+"/style.css");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString style = file.readAll();

    qApp->setStyleSheet(style);

    //РіСЂСѓР·РёРј РёРєРѕРЅРєРё СЃС‚РёР»СЏ
    Ui_VimkaMain *mwUI = m_rosterWindows->ui;

    QString iconsDir = styleDir() + "/icons/";

    online = QIcon(iconsDir + "online.png");
    offline = QIcon(iconsDir + "offline.png");
    connecting = QIcon(iconsDir + "connecting.png");
    incomming = QIcon(iconsDir + "incomming.png");
    minimize = QIcon(iconsDir + "minimize.png");
    maximize = QIcon(iconsDir + "maximize.png");
    quit = QIcon(iconsDir + "quit.png");
    restore = QIcon(iconsDir + "restore.png");
    emoticon = QIcon(styleDir() + "/emoticons/face-in-love.png");
    sound = QIcon(iconsDir + "sound.png");
    profile = QIcon(iconsDir + "profile.png");
    play = QIcon(iconsDir + "play.gif");
    pause = QIcon(iconsDir + "pause.gif");

    markasreadpath = iconsDir + "markasread.png";
    deletemsgpath = iconsDir + "deletemsg.png";
    quotepath = iconsDir + "quote_icon.png";
    sexmalepath = iconsDir + "sex_male.png";
    sexfemalepath = iconsDir + "sex_female.png";
    sexnonepath = iconsDir + "sex_none.png";

    mwUI->tbAbout->setIcon(QIcon(iconsDir + "about.png"));
    mwUI->tbExtended->setIcon(QIcon(iconsDir + "restore.png"));
    mwUI->tbStatus->setIcon(QIcon(iconsDir + "status.png"));
    mwUI->tbLogout->setIcon(QIcon(iconsDir + "logout.png"));
    mwUI->cbStatus->setItemIcon(0, online);
    mwUI->cbStatus->setItemIcon(1, offline);
    mwUI->tbSound->setIcon(sound);
    mwUI->tbSettingsSave->setIcon(QIcon(iconsDir + "status.png"));
    mwUI->tbSettingsCancel->setIcon(QIcon(iconsDir + "chats.png"));
    mwUI->tbShowSettings->setIcon(QIcon(iconsDir + "settings.png"));

    //СЃС‚РёР»СЊ С‡Р°С‚Р°
    QFile h(styleDir()+"/chat/header.htm");
    if (!h.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream hin(&h);
    sheader = hin.readAll();

    QFile m(styleDir()+"/chat/message.htm");
    if (!m.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream min(&m);
    smessage = min.readAll();

    QFile f(styleDir()+"/chat/footer.htm");
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream fin(&f);
    sfooter = fin.readAll();

}

void SettingsManager::applySettings()
{
    QSettings settings(settingsDir()+"/vimka.ini", QSettings::IniFormat);

    settings.beginGroup("Proxy");

    settings.setValue("useProxy",m_rosterWindows->ui->gbProxy->isChecked() );
    settings.setValue("useProxyAuth",m_rosterWindows->ui->gbProxyAuth->isChecked() );
    settings.setValue("proxyType",m_rosterWindows->ui->cbProxyType->currentIndex() );
    settings.setValue("proxyHost",m_rosterWindows->ui->leProxyHost->text() );
    settings.setValue("proxyPort",m_rosterWindows->ui->sbProxyPort->value() );
    if (m_rosterWindows->ui->gbProxyAuth->isChecked()){
        settings.setValue("proxyLogin",m_rosterWindows->ui->leProxyLogin->text() );
        settings.setValue("proxyPassword",m_rosterWindows->ui->leProxyPass->text() );
    }

    settings.endGroup();

    QNetworkProxy::ProxyType pType = QNetworkProxy::NoProxy;
    switch( m_rosterWindows->ui->cbProxyType->currentIndex() ){
    case 0:
        pType = QNetworkProxy::Socks5Proxy;
        break;
    case 1:
        pType = QNetworkProxy::HttpProxy;
        break;
    }

    QNetworkProxy appProxy(pType, m_rosterWindows->ui->leProxyHost->text(),
                           m_rosterWindows->ui->sbProxyPort->value(),
                           m_rosterWindows->ui->leProxyLogin->text(),
                           m_rosterWindows->ui->leProxyPass->text());

    if (m_rosterWindows->ui->gbProxy->isChecked()){
        QNetworkProxy::setApplicationProxy(appProxy);
    }

}
