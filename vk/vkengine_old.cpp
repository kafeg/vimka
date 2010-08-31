#include "vkengine.h"
#include <QHBoxLayout>
#include <QWebView>
#include <QWebPage>
#include <QWebFrame>
#include <QWebElement>
#include <QLineEdit>
#include <QDebug>
#include "vkrequest.h"

VKEngine::VKEngine(VimkaMain *rosterWindow, QObject *parent) :
        QObject(parent), authPageUrl("http://vkontakte.ru/login.php?app=1862639&layout=popup&type=browser&settings=1055")
{
    m_rosterWindow = rosterWindow;

    app_id = 1914138;
    app_key = "MnJ2xvehDeGnn8OnpjWw";
    api_url = "http://api.vkontakte.ru/api.php";

    //= QUrl("http://vkontakte.ru/login.php?app=1862639&layout=popup&type=browser&settings=1055");

    /*
    auth_http = new QHttp("vkontakte.ru", 80);
    connect (auth_http, SIGNAL( responseHeaderReceived ( const QHttpResponseHeader &  ) ),
             this, SLOT( slotResponseHeaderReceived ( const QHttpResponseHeader &  ) ));
    connect (auth_http, SIGNAL( done ( bool ) ), this, SLOT( slotAppAuthDone(bool)));

    auth_user_http = new QHttp("login.vk.com", 80);

    connect (auth_user_http, SIGNAL( responseHeaderReceived ( const QHttpResponseHeader &  ) ),
             this, SLOT( slotResponseUserHeaderReceived ( const QHttpResponseHeader &  ) ));

    connect (auth_user_http, SIGNAL( done ( bool ) ), this, SLOT( slotUserAuthDone(bool)));
*/
    viewBrowser = new QWidget();
    QVBoxLayout *l = new QVBoxLayout(viewBrowser);

    QLineEdit *adress = new QLineEdit();
    l->addWidget(adress);

    debugView = new QWebView();
    l->addWidget(debugView);
    //dbgBrs->show();

    connect(debugView->page(),SIGNAL(loadFinished(bool)),this,SLOT(authPageLoaded(bool)));

    // connect(adress, SIGNAL(textEdited(QString)),this,SLOT(dbgViewLoad(QString)));*/
}

void VKEngine::requestAppAuth(QString userMail, QString userPass)
{
    m_user = userMail;
    m_pass = userPass;

    loggined = false;
    saved =false;

    debugView->load(authPageUrl);

    //requestUserAuth();

    //http://vkontakte.ru/login.php?app=1862639&layout=popup&type=browser&settings=1055

    //debugView->load(QUrl("http://vkontakte.ru/login.php?app=1862639&layout=popup&type=browser&settings=1055&email="+userName+"&pass="+userPass));
    //debugView->load(QUrl("http://login.vk.com?act=login&success_url=&fail_url=&try_to_login=1&to=&vk=&email=v31337@gmail.com&pass=gO1MHsSwGnXXRk&expire="));

}


void VKEngine::authPageLoaded(bool ok)
{
    QWebElement document = debugView->page()->mainFrame()->documentElement();



    if (saved){
        QString url = debugView->url().toString();
        //qDebug () << url;
        if (url.contains("http://vkontakte.ru/api/login_success.html#session=")){
            url.remove("http://vkontakte.ru/api/login_success.html#session=");
            url.remove("{");
            url.remove("}");
            QStringList jsonList = url.split(",");
            foreach(QString elem, jsonList){
                QString key = elem.split(":").at(0);
                key = key.remove("\"");
                QString value = elem.split(":").at(1);
                value = value.remove("\"");
                qDebug () << key << value;

                if (key == "expire"){
                    expire = value;
                }
                if (key == "mid"){
                    mid = value;
                }
                if (key == "secret"){
                    mid = value;
                }
                if (key == "sid"){
                    mid = value;
                }
            }
        }
    }

    if (loggined==true){
        saved = true;
        document.evaluateJavaScript("saveSettings()");
    }else{
        //<input type="email" onkeydown="if (event.keyCode == 10 || event.keyCode == 13) doLogin();" class="inputText" name="email" id="email">

        QWebElement emailTextInput = document.findFirst("input[type=email]");
        emailTextInput.setAttribute("value", m_user);

        //<input type="password" onkeydown="if (event.keyCode == 10 || event.keyCode == 13) doLogin();" class="inputText" autocomplete="off" id="pass">

        QWebElement passTextInput = document.findFirst("input[type=password]");
        passTextInput.setAttribute("value", m_pass);

        loggined = true;
        document.evaluateJavaScript("doLogin()");
    }
}

/*
void VKEngine::requestUserAuth()
{
    //http://login.vk.com?act=login&success_url=&fail_url=&try_to_login=1&to=&vk=&email=sss@gmail.com&pass=111&expire="));

    QByteArray content = "";
    content.append("act");
    content.append("=");
    content.append("login");
    content.append("&");
    content.append("success_url");
    content.append("=");
    content.append("");
    content.append("&");
    content.append("fail_url");
    content.append("=");
    content.append("");
    content.append("&");
    content.append("try_to_login");
    content.append("=");
    content.append("1");
    content.append("&");
    content.append("to");
    content.append("=");
    content.append("");
    content.append("&");
    content.append("vk");
    content.append("=");
    content.append("");
    content.append("&");
    content.append("email");
    content.append("=");
    content.append(m_user);
    content.append("&");
    content.append("pass");
    content.append("=");
    content.append(m_pass);
    content.append("&");
    content.append("expire");
    content.append("=");
    content.append("");

    // header
    QHttpRequestHeader header("POST", "/");
    header.setValue("Host", "login.vk.com");
    header.setContentType("application/x-www-form-urlencoded");
    header.setContentLength(content.length());

    // request
    auth_user_httpId = auth_user_http->request(header, content);
}

void VKEngine::dbgViewLoad(QString adress)
{
   // debugView->load(QUrl(adress));
}

void VKEngine::slotResponseUserHeaderReceived ( const QHttpResponseHeader & resp )
{
    qDebug() << "RESP: " << resp.toString() << "END RESP";
    QStringList cookie_values = resp.allValues("set-cookie");

    foreach(QString val, cookie_values){
        val = val.section(";", 0, 0);
        userAuthCookie.append(val);
}

    qDebug() << "COOKIE" << userAuthCookie;
}

void VKEngine::slotUserAuthDone ( bool error )
{
    debugView->setHtml(auth_user_http->readAll());

    //получаем сессию
    //http://vkontakte.ru/login.php?app=1862639&layout=popup&type=browser&settings=1055

    QByteArray content = "";
    content.append("app");
    content.append("=");
    content.append("1862639");
    content.append("&");
    content.append("layout");
    content.append("=");
    content.append("popup");
    content.append("&");
    content.append("type");
    content.append("=");
    content.append("browser");
    content.append("&");
    content.append("settings");
    content.append("=");
    content.append("1055");

    // header
    QHttpRequestHeader header("POST", "/login.php");
    header.setValue("Host", "vkontakte.ru");
    foreach(QString cookie, userAuthCookie){
        header.addValue("Cookie", cookie);
}
    header.setContentType("application/x-www-form-urlencoded");
    header.setContentLength(content.length());

    // request
    auth_httpId = auth_http->request(header, content);
}

void VKEngine::slotResponseHeaderReceived ( const QHttpResponseHeader & resp )
{
    QString cookie_value = resp.value("set-cookie");
    cookie_value = cookie_value.section(";", 0, 0);
    appAuthCookie = cookie_value; // Store teh cokie value, for the second request
}

void VKEngine::slotAppAuthDone ( bool error )
{
    debugView->setHtml(auth_http->readAll());
    //qDebug() << auth_http->
}
*/
