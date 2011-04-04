#ifndef VIMKAMAIN_H
#define VIMKAMAIN_H

#include <QMainWindow>

namespace Ui {
    class VimkaMain;
}

class VKEngine;
class VKRoster;
class SettingsManager;
class Chats;
class QTreeWidgetItem;
class TrayIcon;
class QDir;
class QSettings;
class QLabel;
class QLineEdit;
class wwActiveLabel;
class wwActiveLineEdit;
class VKRequest;
class QHttpImage;
class AboutDialog;
class SlidingStackedWidget;

class VimkaMain : public QMainWindow
{
    Q_OBJECT
    friend class SettingsManager;
    friend class Chats;
    friend class TrayIcon;
    friend class VKRoster;
public:
    explicit VimkaMain(QWidget *parent = 0);
    ~VimkaMain();
    TrayIcon *trayIcon;
    Chats *chats;
    SettingsManager *settingsMngr;

    QString currentEmail, currentPass, lastPass, lastEmail;

    //управление статусами
    wwActiveLabel *activityLabel;
    wwActiveLineEdit *activityEditor;
    QLineEdit *filterEditor;
    QWidget *rosterWidget;
    SlidingStackedWidget *slidingStacked;

    //upload мигалка
    QWidget *uploadWgt;

    //инфо о пользователе.
    int reqProfileId;
    QString first_name, last_name, photo_rec_url;
    QImage mid_photo_rec;
    QHttpImage *midImg;

protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::VimkaMain *ui;
    VKEngine *vkEngine;
    VKRoster *vkRoster;

    QStringList emails;

    AboutDialog *aboutDialog;

private slots:
    void on_tbShowSettings_clicked();
    void on_tbSettingsSave_clicked();
    void on_tbSettingsCancel_clicked();
    void on_lePass_returnPressed();
    void on_tbAbout_clicked();
    void on_tbStatus_clicked(bool checked);
    void on_tbLogout_clicked();
    void on_pbLogin_clicked();
    void on_pbExit_clicked();
    void actionLogout_triggered();
    void on_leFilter_textChanged(QString cond);
    void actionShowFilter_triggered(bool checked);
    void actionStatus_triggered(bool checked);
    void on_cbMails_currentIndexChanged(int index);
    void on_cbStatus_currentIndexChanged(int index);
    void slotProfileLoaded(VKRequest *req);

    void midPhotoLoaded();

    void vkConnected();
    void vkDisconnected();
};

#endif // VIMKAMAIN_H
