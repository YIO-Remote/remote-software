#ifndef YIOAPI_H
#define YIOAPI_H

#include <QObject>
#include <QQmlApplicationEngine>
#include <QtWebSockets/QWebSocket>
#include <QtWebSockets/QWebSocketServer>

#include "yioapiinterface.h"
//#include "qzeroconf.h"
#include "../qtzeroconf/qzeroconf.h"
#include "config.h"

class YioAPI : public QObject, YioAPIInterface
{
    Q_OBJECT
    Q_INTERFACES(YioAPIInterface)

public:
    Q_PROPERTY  (bool running       READ running            NOTIFY runningChanged)      // returns the state of the API
    Q_PROPERTY  (QString hostname   READ hostname           NOTIFY hostnameChanged)     // returns the hostname of the remote

    Q_INVOKABLE void start();                                                           // start the API
    Q_INVOKABLE void stop();                                                            // stop the API
    //Q_INVOKABLE void sendMessage(QString message);                  // send a message to a client

    // CONFIG MANIPULATION METHODS
    Q_INVOKABLE QVariantMap     getConfig                   ();
    Q_INVOKABLE void            addEntityToConfig           (QVariantMap entity);

    bool running()
    {
        return m_running;
    }

    QString hostname()
    {
        return m_hostname;
    }

    explicit YioAPI(QQmlApplicationEngine *engine = nullptr);
    virtual ~YioAPI();

    static YioAPI*       getInstance     ()
    { return s_instance; }

signals:
    void closed();
    void messageReceived(QVariantMap message);
    void runningChanged();
    void hostnameChanged();

public slots:
    void onNewConnection();
    void processMessage(QString message);
    void onClientDisconnected();

private:
    QWebSocketServer*            m_server;
    QMap<QWebSocket *, bool>     m_clients; // websocket client, true if authentication was successful

    bool                         m_running = false;

    static YioAPI*               s_instance;
    QQmlApplicationEngine*       m_engine;

    QString                      m_token = "0";
    QString                      m_hostname;

    QZeroConf                    m_qzero_conf;
};
#endif // YIOAPI_H
