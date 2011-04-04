#include "vimkamain.h"
#include "ui_vimkamain.h"
#include "chats/personalchat.h"
#include <QCloseEvent>
#include "vk/vkengine.h"
#include <QMovie>
#include <QMessageBox>
#include "vk/vkroster.h"
#include "settingsmanager.h"
#include "chats/chats.h"
#include <QSettings>
#include <QDir>
#include "trayicon.h"
#include "roster/wwactivelabel.h"
#include "roster/wwactivelineedit.h"
#include <QDomNode>
#include <QDomNodeList>
#include <QDomElement>
#include <QDesktopWidget>
#include "roster/qhttpimage.h"
#include "aboutdialog.h"
#include "SlidingStackedWidget.h"

VimkaMain::VimkaMain(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::VimkaMain)
{
    ui->setupUi(this);

    slidingStacked= new SlidingStackedWidget(this);
    slidingStacked->setSpeed(1000);

    //индусо-код... да и пох...
    for (int i = 0; i < ui->stackedWidget->count();i++){
      slidingStacked->addWidget(ui->stackedWidget->widget(i));
    }
    for (int i = 0; i < ui->stackedWidget->count();i++){
      slidingStacked->addWidget(ui->stackedWidget->widget(i));
    }
    for (int i = 0; i < ui->stackedWidget->count();i++){
      slidingStacked->addWidget(ui->stackedWidget->widget(i));
    }

    ui->stackedWidget = slidingStacked;
    setCentralWidget(slidingStacked);

    slidingStacked->setCurrentWidget(ui->loginPage);
    ui->lblErrorLogin->hide();
    ui->listRoster->hide();

    QRect rect = QApplication::desktop()->geometry();
    this->move (rect.center() - this->rect().center());

    //чисто для удобства выносим ссылки на некоторые виджеты...
    activityLabel = new wwActiveLabel(this);
    activityLabel->setWordWrap(true);
    activityLabel->setCursor(QCursor(Qt::PointingHandCursor));
    ui->widgetActivity->layout()->addWidget(activityLabel);
    activityLabel->hide();
    activityEditor = new wwActiveLineEdit(this);
    ui->widgetActivity->layout()->addWidget(activityEditor);
    activityEditor->hide();
    activityEditor->setToolTip(tr("Press Enter for apply activity, or Esc for discard"));
    uploadWgt = ui->uploadWidget;
    filterEditor = ui->leFilter;
    filterEditor->setToolTip(tr("Enter your search phrase here..."));
    rosterWidget = ui->rosterPage;

    settingsMngr = new SettingsManager(this);

    vkEngine = new VKEngine(this);
    trayIcon = new TrayIcon(this);
    chats = new Chats(this);
#ifdef MOBILE_UI
    slidingStacked->addWidget(chats);
#endif

    settingsMngr->loadSettings();

    connect(vkEngine,SIGNAL(connected()),this,SLOT(vkConnected()));
    connect(vkEngine,SIGNAL(disconnected()),this,SLOT(vkDisconnected()));
    trayIcon->setIcon(TrayIcon::Offline);
    connect(vkEngine,SIGNAL(connected()),chats,SLOT(vkConnected()));

    //ростер
    vkRoster = new VKRoster(this,vkEngine, ui->listRoster);

    QMovie *movie = new QMovie(":/images/upload.gif");
    ui->lblUpload->setMovie(movie);
    movie->start();
    ui->uploadWidget->hide();

    if (ui->cbAutoLogin->isChecked()){
        on_pbLogin_clicked();
        ui->cbStatus->setCurrentIndex(0);
        filterEditor->setFocus();
    }

    connect(vkEngine, SIGNAL(requestFinished(VKRequest*)),
            this, SLOT(slotProfileLoaded(VKRequest *)));

    midImg = new QHttpImage(this);
    connect(midImg, SIGNAL(loaded()),
            this,SLOT(midPhotoLoaded()));

    aboutDialog = new AboutDialog(this);

    on_tbStatus_clicked(ui->tbStatus->isChecked());

    setAttribute (Qt::WA_ShowWithoutActivating);
    setWindowFlags (Qt::WindowStaysOnTopHint);
    setFocusPolicy(Qt::NoFocus);
}

VimkaMain::~VimkaMain()
{
    settingsMngr->saveSettings();

    delete ui;
}

void VimkaMain::on_cbStatus_currentIndexChanged(int index)
{
    switch(index){
    case 0:
        vkEngine->requestAppAuth(currentEmail, currentPass);
        ui->uploadWidget->show();
        // ui->cbStatus->setEnabled(false);
        break;
    case 1:
        vkEngine->disconnect();
        ui->uploadWidget->hide();
        ui->cbStatus->setEnabled(true);
        activityLabel->hide();
        chats->hide();
        break;
    }
}

void VimkaMain::on_cbMails_currentIndexChanged(int index)
{
    if (ui->cbMails->itemText(index) == lastEmail) {
        ui->lePass->setText(lastPass);
    }else{
        ui->lePass->setText("");
    }
}

void VimkaMain::closeEvent(QCloseEvent *event)
{
    if (trayIcon->trayIcon->isVisible()) {
        hide();
        event->ignore();
    }
}

void VimkaMain::vkConnected()
{
    ui->uploadWidget->hide();
    ui->cbStatus->setEnabled(true);

    reqProfileId = vkEngine->reqGetProfiles(QStringList() << vkEngine->mid,
                                            QStringList() << "first_name"
                                            << "last_name" << "photo_rec");
}

void VimkaMain::vkDisconnected()
{
    //QMessageBox::information(0,tr(""),tr("Unable to connect!"));
    ui->uploadWidget->hide();
    ui->cbStatus->setEnabled(true);
    activityLabel->hide();
    ui->cbStatus->setCurrentIndex(1);
    ui->lblUserName->setText(tr("Vimka"));
}

void VimkaMain::actionStatus_triggered(bool checked)
{
    vkRoster->setShowOnlyOnline(checked);
}

void VimkaMain::actionShowFilter_triggered(bool checked)
{
    ui->leFilter->setVisible(checked);
}

void VimkaMain::on_leFilter_textChanged(QString cond)
{
    vkRoster->findUser(cond);
}

void VimkaMain::actionLogout_triggered()
{
    settingsMngr->saveSettings();
    if (vkEngine->isAppConnected){
        vkEngine->disconnect();
    }

    chats->hide();
    chats->clearList();

    slidingStacked->slideInIdx(slidingStacked->indexOf(ui->loginPage));
    ui->lblErrorLogin->hide();
    ui->lePass->clear();
    currentEmail.clear();
    currentPass.clear();
    lastEmail.clear();
    lastPass.clear();
    ui->cbAutoLogin->setChecked(false);
    slidingStacked->slideInIdx(slidingStacked->indexOf(ui->loginPage));

}

void VimkaMain::on_pbExit_clicked()
{
    qApp->quit();
}

void VimkaMain::on_pbLogin_clicked()
{
    currentPass = ui->lePass->text();
    currentEmail = ui->cbMails->currentText();
    ui->lblErrorLogin->hide();

    if (currentEmail.isEmpty() || !currentEmail.contains("@") ||
        currentPass.isEmpty()){
        ui->lblErrorLogin->show();
        return;
    }

    lastEmail = currentEmail;
    lastPass = currentPass;

    if (!emails.contains(currentEmail)){
        emails.append(currentEmail);
    }

    slidingStacked->setCurrentWidget(ui->rosterPage);
    filterEditor->setFocus();

    settingsMngr->saveSettings();
}

void VimkaMain::slotProfileLoaded(VKRequest *req)
{
    if (reqProfileId!=req->reqId ||
        req->reqType != VKRequest::getProfiles){
        return;
    }

    QDomDocument doc("Profiles");
    doc.setContent(req->result);
    QDomElement docElem = doc.documentElement();
    QDomElement userElem = docElem.elementsByTagName ( "user" ).at(0).toElement();

    first_name = userElem.elementsByTagName("first_name").at(0).toElement().text();
    last_name = userElem.elementsByTagName("last_name").at(0).toElement().text();
    photo_rec_url = userElem.elementsByTagName("photo_rec").at(0).toElement().text();

    midImg->setUrl(photo_rec_url);
    midImg->load();

    ui->lblUserName->setText(first_name + " " + last_name);

}

void VimkaMain::midPhotoLoaded()
{
    mid_photo_rec = midImg->getImage();
}

void VimkaMain::on_tbLogout_clicked()
{
    if (QMessageBox::warning(this,tr("Logout?"),
                             tr("Do you really want to conclude this session and reset the last stored password?"),
                             QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes){
        vkEngine->disconnect();
        actionLogout_triggered();
    }
}

void VimkaMain::on_tbStatus_clicked(bool checked)
{
    vkRoster->setShowOnlyOnline(checked);
}

void VimkaMain::on_tbAbout_clicked()
{
    QRect rect = QApplication::desktop()->geometry();
    QPoint pos = rect.center() - aboutDialog->rect().center();
    aboutDialog->move(pos);
    aboutDialog->exec();
}

void VimkaMain::on_lePass_returnPressed()
{
    on_pbLogin_clicked();
}

void VimkaMain::on_tbSettingsCancel_clicked()
{
    slidingStacked->slideInIdx(slidingStacked->indexOf(ui->rosterPage));
}

void VimkaMain::on_tbSettingsSave_clicked()
{
    slidingStacked->slideInIdx(slidingStacked->indexOf(ui->rosterPage));
    settingsMngr->applySettings();
}

void VimkaMain::on_tbShowSettings_clicked()
{
    slidingStacked->slideInIdx(slidingStacked->indexOf(ui->settingsPage));
}
