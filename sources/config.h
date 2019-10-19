#ifndef CONFIG_H
#define CONFIG_H

#include <QtDebug>

#include <QObject>
#include <QQmlApplicationEngine>
#include <QJsonArray>
#include "configinterface.h"
#include "jsonfile.h"

class Config : public QObject, ConfigInterface
{
    Q_OBJECT
    Q_INTERFACES(ConfigInterface)

public:
    Q_PROPERTY (QVariantMap      read           READ read           NOTIFY configChanged)
    Q_PROPERTY (QVariantMap      write          WRITE readWrite     NOTIFY configChanged)
    Q_PROPERTY (QString          profile        READ profile        WRITE profileWrite      NOTIFY profileChanged)

    Q_INVOKABLE void readConfig(QString path);
    Q_INVOKABLE void writeConfig();

    QVariantMap read()
    {
        return m_config;
    }

    void readWrite(QVariantMap config)
    {
        m_config = config;
        emit configChanged();
    }

    // profile
    QString profile()
    {
        return m_config.value("ui_config").toMap().value("selected_profile").toString();
    }

    void profileWrite(QString id)
    {
        QVariantMap p = m_config.value("ui_config").toMap();
        p.insert("selected_profile", id);
        m_config.insert("ui_config", p);

        writeConfig();

        emit profileChanged();
    }

public:
    explicit Config(QQmlApplicationEngine *engine = nullptr, QString path = "");
    virtual ~Config();

    static Config*       getInstance     ()
    { return s_instance; }

signals:
    void configChanged();
    void profileChanged();

private:
    static Config*               s_instance;
    QQmlApplicationEngine*       m_engine;

    QVariantMap                  m_config;

    JsonFile*                    m_jsf = new JsonFile();
};

#endif // CONFIG_H
