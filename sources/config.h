#ifndef CONFIG_H
#define CONFIG_H

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
    Q_PROPERTY (QVariantMap      read          READ read            NOTIFY configChanged)
    Q_PROPERTY (QVariantMap      write         WRITE readWrite      NOTIFY configChanged)

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

public:
    explicit Config(QQmlApplicationEngine *engine = nullptr, QString path = "");
    virtual ~Config();

    static Config*       getInstance     ()
    { return s_instance; }

signals:
    void configChanged();

private:
    static Config*               s_instance;
    QQmlApplicationEngine*       m_engine;

    QVariantMap                  m_config;

    JsonFile*                    m_jsf = new JsonFile();
};

#endif // CONFIG_H
