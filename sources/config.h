#ifndef CONFIG_H
#define CONFIG_H

#include <QtDebug>

#include <QObject>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QJsonArray>
#include "configinterface.h"
#include "jsonfile.h"

class Config : public QObject, ConfigInterface
{
    Q_OBJECT
    Q_INTERFACES(ConfigInterface)

public:
#ifdef CONFIG_COMPATIBLE
    Q_PROPERTY (QVariantMap      read           READ read                           NOTIFY configChanged)
    Q_PROPERTY (QVariantMap      write                          WRITE readWrite     NOTIFY configChanged)
    QVariantMap read()
    {  return m_config;
    }
    void readWrite(QVariantMap config)
    {
        m_config = config;
        syncCache();
        emit configChanged();
    }
#endif
    Q_PROPERTY (QVariantMap      config         READ config     WRITE setConfig     NOTIFY configChanged)
    Q_PROPERTY (QString          profile        READ profile    WRITE setProfile    NOTIFY profileChanged)
    Q_PROPERTY (QVariantMap      settings       READ getSettings)
    Q_PROPERTY (QVariantMap      integrations   READ getIntegrations)
    Q_PROPERTY (QVariantMap      entities       READ getAllEntities)
    Q_PROPERTY (QVariantMap      settings       READ getSettings)
    Q_PROPERTY (QVariantMap      profiles       READ getProfiles)
    Q_PROPERTY (QVariantMap      ui_config      READ getUIConfig)
    Q_PROPERTY (QVariantMap      pages          READ getPages)
    Q_PROPERTY (QVariantMap      groups         READ getGroups)

    Q_INVOKABLE void readConfig(QString path);
    Q_INVOKABLE void writeConfig();

    // Shortcuts to get the config items, and to decouple a bit from Json structure
    // Please avoid old access via read property
    QVariantMap getIntegrations() {
        return m_config["integrations"].toMap();
    }
    Q_INVOKABLE QVariantMap getIntegration(const QString& type) {
        return getIntegrations().value(type).toMap();
    }
    QVariantMap getAllEntities() {
        return m_config["entities"].toMap();
    }
    Q_INVOKABLE QVariantList getEntities(const QString& type) {
        return getAllEntities().value(type).toList();
    }

    // Assuming that the following config items are accessed quite often they are cached
    QVariantMap getSettings() {
        return m_cacheSettings;
    }
    QVariantMap getUIConfig() {
        return m_cacheUIConfig;
    }
    QVariantMap getProfiles() {
        return m_cacheUIProfiles;
    }
    Q_INVOKABLE QVariantMap getProfile(const QString& profile) {
        return m_cacheUIProfiles.value(profile).toMap();
    }
    QVariantMap getPages() {
        return m_cacheUIPages;
    }
    Q_INVOKABLE QVariantMap getPage(const QString& pageId) {
        return m_cacheUIPages.value(pageId).toMap();
    }
    QVariantMap getGroups() {
        return m_cacheUIGroups;
    }
    Q_INVOKABLE QVariantMap getGroup(const QString& groupId) {
        return m_cacheUIGroups.value(groupId).toMap();
    }

    // The selected, cached profile
    Q_INVOKABLE QVariantMap getProfile() {
        return m_cacheUIProfile;
    }
    Q_INVOKABLE QStringList getProfileFavorites() {
        return m_cacheUIProfile.value("favorites").toStringList();
    }
    Q_INVOKABLE QStringList getProfilePages() {
        return m_cacheUIProfile.value("pages").toStringList();
    }

    // Removed from entities
    Q_INVOKABLE void setFavorite (const QString& entityId, bool value);


    QVariantMap config()
    {
        return m_config;
    }
    void setConfig(const QVariantMap& config)
    {
        m_config = config;
        syncCache();
        emit configChanged();
    }
    QVariant getContextProperty (const QString& name)
    {
        return m_engine->rootContext()->contextProperty(name);
    }

    // get a QML object, you need to have objectName property of the QML object set to be able to use this
    QObject *getQMLObject(QList<QObject*> nodes,const QString &name);
    QObject *getQMLObject(const QString &name);

    // profile
    QString profile()
    {
        return m_cacheProfile;
    }

    void setProfile(QString id)
    {
        QVariantMap p = getUIConfig();
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
    void syncCache ();

    static Config*              s_instance;
    QQmlApplicationEngine*      m_engine;

    QVariantMap                 m_config;

    JsonFile*                   m_jsf;

    // Caches to improve performance
    QString                     m_cacheProfile;
    QVariantMap                 m_cacheSettings;
    QVariantMap                 m_cacheUIConfig;
    QVariantMap                 m_cacheUIProfile;       // of selected Profile
    QVariantMap                 m_cacheUIProfiles;
    QVariantMap                 m_cacheUIPages;
    QVariantMap                 m_cacheUIGroups;
};

#endif // CONFIG_H
