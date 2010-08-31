#ifndef VKENGINE_H
#define VKENGINE_H

#include <QObject>
#include <QHttpResponseHeader>
#include <QHttp>
#include <QHttpRequestHeader>
#include <QUrl>

class VimkaMain;
class QWebView;

class VKEngine : public QObject
{
    Q_OBJECT
public:
    explicit VKEngine(VimkaMain *rosterWindow, QObject *parent = 0);

    uint app_id;
    QString app_key, api_url;

    QString expire, mid, secret, sid;

    void requestAppAuth(QString userMail, QString userPass);

    QWidget *viewBrowser;
signals:

public slots:

private slots:
    void authPageLoaded(bool ok);/*
    void slotResponseUserHeaderReceived ( const QHttpResponseHeader & resp );
    void slotUserAuthDone ( bool error );

    void slotResponseHeaderReceived ( const QHttpResponseHeader & resp );
    void slotAppAuthDone ( bool error );*/
private:
    VimkaMain *m_rosterWindow;
    QString m_user, m_pass;
    QUrl authPageUrl;

    bool loggined, saved;

    //void requestUserAuth();

    //desktop auth
    /*
    QHttp *auth_user_http, *auth_http;
    int auth_user_httpId, auth_httpId;
    QString appAuthCookie;
    QStringList userAuthCookie;*/

    QWebView *debugView;

};

#endif // VKENGINE_H
