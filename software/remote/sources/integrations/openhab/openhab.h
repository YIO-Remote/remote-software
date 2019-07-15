#ifndef OPENHAB_H
#define OPENHAB_H

//#include <QtPlugin>
#include <QObject>
#include <QString>
#include <QColor>
#include <QVariant>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "../integration.h"
#include "../integrationinterface.h"

class OpenHAB : public Integration, IntegrationInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "YIO.IntegrationInterface" FILE "openhab.json")
    Q_INTERFACES(IntegrationInterface)

public:
    explicit OpenHAB();

    Q_INVOKABLE void initialize	    (int integrationId, const QVariantMap& config, QObject *entities) override;
    Q_INVOKABLE void connect	    () override;
    Q_INVOKABLE void disconnect	    () override;

signals:
    void notify();

public slots:
     void sendCommand                (const QString& type, const QString& id, const QString& command, const QVariant& param) override;
     void processResponse            (QNetworkReply* reply);
     void onTimeout                  ();

private:
    void updateEntity               (const QString& entity_id, const QVariantMap& attr) override;
    void updateLight                (Entity* entity, const QVariantMap& attr);
    void updateBlind                (Entity* entity, const QVariantMap& attr);

    void getRequest                 (const QString& url);

    QTimer                          m_polling_timer;

    EntitiesInterface*  m_entities;

    QString 			m_ip;
    int                 m_polling_interval=1000; // default 1 second

};

#endif // OPENHAB_H
