#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QApplication>
#include <QIcon>

class VimkaMain;
class SettingsManager : public QObject
{
    Q_OBJECT
public:
    explicit SettingsManager(VimkaMain *rosterWindows, QObject *parent = 0);

    void loadSettings();
    void saveSettings();
    void applySettings();
    QString settingsDir(); //каталог настроек
    QString dataDir(); //текущий каталог с данными
    QString styleDir(); //каталог текущего стиля
    QString stylesDir(); //каталог со всеми стилями
    void loadStyle(); //грузит текущий стиль
    //иконки стиля
    QIcon online, offline, connecting, incomming, minimize, maximize,
    quit, restore, emoticon, sound, profile, play, pause;
    QString sheader, smessage, sfooter, markasreadpath, deletemsgpath, quotepath, sexmalepath, sexfemalepath, sexnonepath;
signals:

public slots:

private:
    VimkaMain *m_rosterWindows;
    QString currentStyle;
};

#endif // SETTINGSMANAGER_H
