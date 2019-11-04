#ifndef YIOAPI_H
#define YIOAPI_H

#include <QObject>
#include <QQmlApplicationEngine>
#include <QtWebSockets/QWebSocket>
#include <QtWebSockets/QWebSocketServer>
#include <QCryptographicHash>

#include "yioapiinterface.h"
//#include "qzeroconf.h"
#include "../qtzeroconf/qzeroconf.h"
#include "config.h"
#include "integrations/integrations.h"

class YioAPI : public YioAPIInterface
{
    Q_OBJECT
    Q_INTERFACES(YioAPIInterface)

public:
    Q_PROPERTY  (bool running       READ running            NOTIFY runningChanged)      // returns the state of the API
    Q_PROPERTY  (QString hostname   READ hostname           NOTIFY hostnameChanged)     // returns the hostname of the remote

    Q_INVOKABLE void start() override;                                                           // start the API
    Q_INVOKABLE void stop() override;                                                            // stop the API
    Q_INVOKABLE void sendMessage(QString message) override;                                      // send a message to all clients


    // CONFIG MANIPULATION METHODS
    Q_INVOKABLE QVariantMap     getConfig                   () override;
    Q_INVOKABLE void            setConfig                   (QVariantMap config);
    Q_INVOKABLE bool            addEntityToConfig           (QVariantMap entity) override;


    // NETWORK SERVICES DISCOVERY
    Q_PROPERTY  (QVariantList   discoveredServices          READ discoveredServices     NOTIFY discoveredServicesChanged)
    Q_INVOKABLE void            discoverNetworkServices     () override;
    Q_INVOKABLE void            discoverNetworkServices     (QString mdns) override;

    QVariantList discoveredServices();


    bool running()
    {
        return m_running;
    }

    QString hostname()
    {
        return m_hostname;
    }

    explicit YioAPI(QQmlApplicationEngine *engine = nullptr);
    virtual ~YioAPI() override;

    static YioAPI*              getInstance()
    { return s_instance; }

signals:
    void closed();
    void messageReceived(QVariantMap message);
    void runningChanged();
    void hostnameChanged();
    void discoveredServicesChanged();
//    void serviceDiscovered(QMap<QString, QVariantMap> services);

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

    QString                      m_token = "c82b5fd6bea6fc3faf9a30bb864a9ee2";
    QByteArray                   m_hash = "{U\xC0<$\xF7\n\xA7PA\xC3=\xBEk\xF5\xC1\xCA\x8B\t\x91\xA0\x9Et\xBA""E\xE9\xA0)\xE4\x07^E\x04\x17Xg\xE4)\x04\xB7\xD4\x9D,\x19%\xD7\xA1\xDC\x84U\x83\xA2\xAA\x1D\xD7:\xBE\xF6""1\xFA\x90\xED\x16\xBB";//QCryptographicHash::hash(m_token.toLocal8Bit(), QCryptographicHash::Sha512);
    QString                      m_hostname;

    QZeroConf                    m_qzero_conf;
    QZeroConf*                   m_qzero_conf_browser;

    QStringList                  m_discoverableServices;
    QMap<QString, QVariantMap>   m_discoveredServices; // name as string, <ip address as string, mdns name as string>  "192.169.100.1", 496
};

#endif // YIOAPI_H
