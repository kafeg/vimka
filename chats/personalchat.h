#ifndef PERSONALCHAT_H
#define PERSONALCHAT_H

#include <QWidget>
#include <QUrl>

namespace Ui {
    class PersonalChat;
}

class VKMessage;
class Chats;
class VKEngine;
class VKRequest;
class QWebView;
class ChatTextEdit;
class QHttpImage;
class QTabWidget;
class QTimer;
class QStandardItemModel;
class MessageStyler;
class MediaPlayer;
class AlbumsModel;
class VideoListModel;
class SettingsManager;
class PhotoViewer;

class PersonalChat : public QWidget
{
    Q_OBJECT
    friend class Chats;
public:
    explicit PersonalChat( QString _fromId, Chats *chatsWindow,
                           VKEngine *engine, MediaPlayer *pagePlayer, AlbumsModel *albumsModel);
    ~PersonalChat();

    bool showed;

    bool hasNewMessages;
    QString fromId;
    Chats *m_Chats;
    VKEngine *m_vkEngine;

    void messageReceived(VKMessage *incommingMessage);

    void showChat();

    void focusInEvent(QFocusEvent *e);

    bool isProfilesLoaded;

    void hideChat(  )  ;
    void showEvent(QShowEvent *e);

signals:
    //void incMessNotificationStoped(int tabId);
    //void profileLoaded(QString fromId);

public slots:
    void startMigalko();
    void stopMigalko();

    void currentChatChanged(int id);

    void messageStyled(VKMessage *incommingMessage);

private:
    Ui::PersonalChat *ui;
    SettingsManager *sMn;

    MediaPlayer *pagePlayer;

    QTimer *incMessageMigalko;

    bool onlineMess;

    //views
    QWebView *webView;
    ChatTextEdit *chatEdit;

    //history
    QList<VKMessage *> chatHistory;
    int reqGetProfilesId;

    //QString last;
    //определяем по флагам из VKMessage чьё сообщение было перед
    //текущим обрабатываемым. Для смены стилей.

    //profilesInfo
    QString fromFullName, fromFirstName, fromLastName, fromPhotoUrl,
    midFullName, midFirstName, midLastName, midPhotoUrl;

    int reqSendId;

    //помечаем как прочитанное...
    QTimer *markFromIdMessagesAsReadTimer;
    int markResponsReqId;
    QStringList unreadedFromIdMids, unreadedFromIdLocalIds; //оптимизация хренимизация... как там корень всех бед вроде?

    //подгрузка истории
    int getHistoryId, historyPartCount;
    QString historyAllCount;

    //подгрузка песенок
    int getAudioId;
    QStandardItemModel *audioItems;

    //обработка тела сообщения
    MessageStyler *bodyStyler;

    //фотки
    AlbumsModel *albumsModel;

    //видео
    VideoListModel *videoModel;

    //скроллер
    QTimer *viewScroller;

    //првоерка доставки
    void disableEmlements();
    void enableEmlements();
    QTimer *sendFailTimer;

    void urlClicked(QUrl url);
    int reqAudioAddId;
    QString reqAudioAddAid;

    int reqFromActivityGetId, reqMessagesDeleteId;

    //прсомотрщик пришедшей фотки (ага тоже декларативный)
    PhotoViewer *photoViewer;

private slots:
    //LPS Online/Offline
    void on_pbPVback_clicked();
    //void on_tbOpenFromIdPage_clicked();
    void slotLps8UserOnline(QString uid, QString flags);
    void slotLps9UserOffline(QString uid, QString flags);

    void on_pbSendVideo_clicked();
    void on_pbCancelPostVideo_clicked();
    void on_tbSendVideo_clicked();
    void on_pbCancelSendPhoto_clicked();
    void on_tbAddPhoto_clicked();
    void on_pbAudioSend_clicked();
    void on_tbAddAudio_clicked();
    void on_pbCancelPostAudio_clicked();
    void on_sendButton_clicked();

    void slotProfilesLoaded(VKRequest *req);

    void slotFailedToSendMessage();

    void onIncTimer();

    void updateView();
    QString makeHtmlMess(VKMessage *mess);
    void messageChanged(VKMessage *mess);

    //помечаем как прочитанные...
    void markFromIdMessagesAsReadStart();
    void markFromIdMessagesAsReadFinish();
    void markResponsReceived(VKRequest *req);

    //для подгрузки истории
    void startDownloadNextPartOfHistory(QUrl url);
    void getHistoryFinished(VKRequest *req);

    //для подгрузки списка песенок...
    void audioGetFinished(VKRequest *req);

    //фотки
    void photoIsSelected(QString preview, QString hq,
                         QString pid, QString aid, QString mid,
                         QString fromId);

    void scrollToBottom();

    //видео
    void videoModelItemsReceived();
    void insertEmoticon(const QString &str);

    //флуд контроль
    void sendMessageReceived(VKRequest *req);

    //аудио добавление
    void receivedAudio_add(VKRequest *req);

    //void receivedFromActivity(VKRequest *req);

    //дополнительно
    void receivedDeleteMessage(VKRequest *req);

    void on_rosterButton_clicked();
};

#endif // PERSONALCHAT_H
