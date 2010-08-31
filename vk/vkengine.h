#ifndef VKENGINE_H
#define VKENGINE_H

#include <QObject>
#include <QUrl>
#include "vkrequest.h"

class VimkaMain;
class QWebView;
class QTimer;
class QTime;
class VKRequest;
class QHttp;
class VKMessage;

class VKEngine : public QObject
{
    Q_OBJECT
public:
    explicit VKEngine(VimkaMain *rosterWindow, QObject *parent = 0);

    //--------------------------------------служебные переменные
    QString app_id, app_key, api_url, expire, mid, secret, sid;

    //----------------------------------------авторизация и дисконнект
    void requestAppAuth(QString userMail, QString userPass);
    void disconnect();

    QWidget* viewBrowser;
    bool loggined, isAppConnected;

    VimkaMain *m_rosterWindow;

    VKRequest *getEmptyReq(VKRequest::Request reqType) { VKRequest *request = new VKRequest(reqType, mid, sid, secret, app_id, -1 ); return request; }

    //------------------------------------------------методы АПИ

    //пользователи

    //    isAppUser – возвращает установил пользователь данное приложение или нет.
    //    getProfiles – возвращает расширенную информацию о пользователях.
    int reqGetProfiles(QStringList uids, QStringList fields);
    //    getUserBalance – возвращает баланс текущего пользователя в данном приложении.
    //    getUserSettings – возвращает настройки приложения текущего пользователя.
    //    getGroups – возвращает список id групп, в которых состоит текущий пользователь.
    int reqGetGroups();
    //    getGroupsFull – возвращает базовую информацию о группах, в которых состоит текущий пользователь.

    //Друзья
    //    friends.get – возвращает список id друзей пользователя.
    int reqFriends_get(QStringList fields, QString name_case = "nom");
    //    friends.getAppUsers – возвращает список id друзей пользователя, которые установили данное приложение.

    //Личный статус
    //    activity.getnew – возвращает текущий статус пользователя.
    int reqActivity_get(QString uid);

    //    activity.setnew – устанавливает статус текущего пользователя.
    int reqActivity_set(QString text);

    //    activity.getHistorynew – возвращает историю статусов.
    //    activity.deleteHistoryItemnew – удаляет элемент истории статусов у текущего пользователя.
    //    activity.getNewsnew – возвращает обновления статусов друзей.

    //    Фотографии
    //    photos.getAlbums – возвращает список альбомов пользователя.
    int reqPhotos_getAlbums(QString uid, QStringList aids = QStringList());
    //    photos.get – возвращает список фотографий в альбоме.
    //    photos.getById – возвращает информацию о фотографиях.
    int reqPhotos_getById(QStringList photos);
    //    photos.createAlbum – создает пустой альбом для фотографий.
    //    photos.editAlbum – обновляет данные альбома для фотографий.
    //    photos.getUploadServer – возвращает адрес сервера для загрузки фотографий.
    //    photos.save – сохраняет фотографии после успешной загрузки.
    //    photos.getProfileUploadServernew – возвращает адрес сервера для загрузки фотографии на страницу пользователя.
    //    photos.saveProfilePhotonew – сохраняет фотографию страницы пользователя после успешной загрузки.
    //    photos.movenew - переносит фотографию из одного альбома в другой.
    //    photos.makeCovernew - делает фотографию обложкой альбома.
    //    photos.reorderAlbumsnew - меняет порядок альбома в списке альбомов пользователя.
    //    photos.reorderPhotosnew - меняет порядок фотографий в списке фотографий альбома.

    //аудио
    //    Аудиозаписи
    //    audio.get – возвращает список аудиозаписей пользователя или группы.
    int reqAudio_get(QString uid, QString gid = "-1", QString aids = "-1",
                     QString need_user = "0");
    //    audio.getById – возвращает информацию об аудиозаписях по их идентификаторам.
    int reqAudio_getById(QStringList audios);
    //    audio.getLyrics - возвращает текст аудиозаписи.
    //    audio.getUploadServer – возвращает адрес сервера для загрузки аудиозаписей.
    //    audio.save – сохраняет аудиозаписи после успешной загрузки.
    //    audio.search – осуществляет поиск по аудиозаписям.
    //    audio.add – копирует существующую аудиозапись на страницу пользователя или группы.
    int reqAudio_add(QString aid, QString oid, QString gid = "-1");
    //    audio.delete – удаляет аудиозапись со страницы пользователя или группы.
    //    audio.editnew – редактирует аудиозапись пользователя или группы.
    //    audio.restorenew – восстанавливает удаленную аудиозапись пользователя или группы.
    //    audio.reordernew – изменяет порядок аудиозаписи в списке аудиозаписей пользователя.

//    Видеозаписи
//    video.get – Возвращает информацию о видеозаписях.
    int reqVideo_get(QString uid, QStringList videos = QStringList(),
                     QString width = "160", QString count = "-1",
                     QString offset = "0");
//    video.editnew– редактирует данные видеозаписи на странице пользователя.
//    video.addnew– копирует видеозапись на страницу пользователя.
//    video.deletenew– удаляет видеозапись со страницы пользователя.
//    video.searchnew– возвращает список видеозаписей в соответствии с заданным критерием поиска.
//    video.getUserVideosnew– возвращает список видеозаписей, на которых отмечен пользователь.
//    video.getCommentsnew– возвращает список комментариев к видеозаписи.
//    video.createCommentnew– создает новый комментарий к видеозаписи.
//    video.editCommentnew– изменяет текст комментария к видеозаписи.
//    video.deleteCommentnew– удаляет комментарий к видеозаписи.
//    video.getTagsnew– возвращает список отметок на видеозаписи.
//    video.putTagnew– добавляет отметку на видеозапись.
//    video.removeTagnew– удаляет отметку с видеозаписи.
//    video.savenew – возвращает данные, необходимые для загрузки видеозаписей, а также данные видеозаписи.

    //Личные сообщения
    //    messages.get – возвращает список входящих либо исходящих сообщений текущего пользователя.
    int reqMessages_get(QString offset, QString count = "10", QString out = "0",
                        QString filters = "0", QString preview_length = "0",
                        QString time_offset = "0");
    //    messages.getDialogs – возвращает список диалогов текущего пользователя.
    //    messages.search – возвращает найденные сообщения текущего пользователя по введенной строке поиска.
    //    messages.send – посылает сообщение.
    int reqMessages_send(VKMessage *message);

    //    messages.delete – удаляет сообщение.
    int reqMessages_delete(QString mid);
    //    messages.restore – восстанавливает только что удаленное сообщение.
    //    messages.markAsNew – помечает сообщения, как непрочитанные.
    //    messages.markAsRead – помечает сообщения, как прочитанные.
    int reqMessages_markAsRead(QStringList mids);

    //    messages.getHistory – возвращает историю сообщений для данного пользователя.
    int reqMessages_getHistory(QString uid, QString offset, QString count = "10");
    //    messages.getLongPollServer – возвращает данные, необходимые для подключения к LongPoll серверу
    int reqMessages_getLongPollServer();

    //-----------------------------------------------------------отстутсвует в api
    //методы исполняемые через debugView - страничку с готовыми кукисами вконтакта
    //результаты методов - веб-страницы, которые ещё нужно распарсить
    enum HTTPRequest{
        //видео
        Video_getUrl,
        //вход
        Login
    };
    //html video parsing, выбираем из странички с видео ссылку на файл
    int reqVideoUrl(QString videoInfo);
    HTTPRequest httpReqType;

signals:
    void requestFinished(VKRequest *req);
    void connected();
    void disconnected();

    //LPS
    void lps0DeleteMessage(QString local_id);
    void lps1ReplaceMessageFlags(QString local_id, int flags);
    void lps2SetMessageFlags(QString local_id, int mask);
    void lps3ResetMessageFlags(QString local_id, int mask);
    void lps4MessageReceived(VKMessage *message);

    void lps8UserOnline(QString uid, QString flags);
    void lps9UserOfline(QString uid, QString flags);

    //отсутствует в апи
    void videoUrlExtracted(QUrl url, bool isBroken);

public slots:

private slots:
    void authPageLoaded(bool ok);
    void slotShowLoginPage();
    void slotAuthPageFailedLoad();
    void slotConnected();

    //очередь запросов
    void slotReqFinished(bool ok);
    void execNextRequest();

    //обработка служебных вернувшихся запросов
    void slotLongPollServerReceived(VKRequest *req);

    //LPS события
    void slotLPSRequestReceived(bool ok);

    //отсутствует в апи
    void videoUrlReceived();

private:
    QString m_user, m_pass;
    QUrl authPageUrl;

    QTimer *loginTimer, *authPageTimer;

    /*окошко авторизации
    а ещё в нём есть драгоценные кукисы =)))
    так что будем пользоваться для прямого забора информаци из
    html страничек вконтакта =\\
    например для выбора ссылки на видео файл*/
    QWebView *debugView;

    //очередь запросов к серверу на исполнение
    VKRequest *m_currentRequest; //текущий выполняемый
    bool m_requestInAction; //пометка что в даный момент выполняется запрос
    QList<VKRequest *> requests; //очередь на выполнение
    void appendRequest(VKRequest *req); //добавлеие нового запроса
    uint m_reqCounter; //общее количество выполненных запросов
    QHttp *m_reqHttp; //объект выполняющий запросы
    QTime *reqInterval; //в секунду максимум три запроса =(

    //longPollServer
    //QHttp *m_reqLPSHttp;
    int reqIdLPS;
    QString keyLPS, serverLPS, tsLPS, urlLPS, lpsResult, waitLPS;
    QWebView *lpsView;

};

#endif // VKENGINE_H
