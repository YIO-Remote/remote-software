/******************************************************************************
 *
 * Copyright (C) 2020 Markus Zehnder <business@markuszehnder.ch>
 * Copyright (C) 2018-2019 Marton Borzak <hello@martonborzak.com>
 *
 * This file is part of the YIO-Remote software project.
 *
 * YIO-Remote software is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * YIO-Remote software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with YIO-Remote software. If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *****************************************************************************/
#pragma once

#include <QJsonArray>
#include <QObject>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtDebug>

#include "jsonfile.h"
#include "yio-interface/configinterface.h"

class Config : public QObject, public ConfigInterface {
    Q_OBJECT
    Q_INTERFACES(ConfigInterface)

    Q_PROPERTY(bool valid READ isValid CONSTANT)
    Q_PROPERTY(QString error READ getError CONSTANT)
    Q_PROPERTY(QVariantMap config READ getConfig WRITE setConfig NOTIFY configChanged)
    Q_PROPERTY(QString profileId READ getProfileId WRITE setProfileId NOTIFY profileIdChanged)
    Q_PROPERTY(QStringList profileFavorites READ profileFavorites NOTIFY profileFavoritesChanged)
    Q_PROPERTY(QVariantMap settings READ getSettings WRITE setSettings NOTIFY settingsChanged)
    Q_PROPERTY(QVariantMap profiles READ getProfiles NOTIFY profilesChanged)
    Q_PROPERTY(QVariantMap ui_config READ getUIConfig WRITE setUIConfig NOTIFY uiConfigChanged)
    Q_PROPERTY(QVariantMap pages READ getPages NOTIFY pagesChanged)
    Q_PROPERTY(QVariantMap groups READ getGroups NOTIFY groupsChanged)
    Q_PROPERTY(UnitSystem unitSystem READ getUnitSystem WRITE setUnitSystem NOTIFY unitSystemChanged)

 public:
    // Common configuration keys. Initialized in config.cpp
    // HELP: anyone knows how to properly define "static const QString" constants across Qt plugin boundaries?
    static const QString KEY_ID;
    static const QString KEY_FRIENDLYNAME;
    static const QString KEY_ENTITY_ID;
    static const QString KEY_AREA;
    static const QString KEY_INTEGRATION;
    static const QString KEY_SUPPORTED_FEATURES;
    static const QString KEY_TYPE;
    static const QString KEY_MDNS;
    static const QString KEY_WORKERTHREAD;
    static const QString OBJ_DATA;

    Q_ENUM(UnitSystem)

    // valid
    bool isValid() const { return m_error.isEmpty(); }

    // error
    QString getError() const { return m_error; }

    // config
    QVariantMap getConfig() override { return m_config; }
    void        setConfig(const QVariantMap& config) override;

    // profile Id
    QString getProfileId() { return m_cacheProfileId; }
    void    setProfileId(QString id);

    // profile favorites
    QStringList profileFavorites() { return m_cacheUIProfile.value("favorites").toStringList(); }

    // settings
    QVariantMap getSettings() override { return m_cacheSettings; }
    void        setSettings(const QVariantMap& config);

    // profiles
    QVariantMap getProfiles() { return m_cacheUIProfiles; }
    void        setProfiles(const QVariantMap& config);

    // ui_config
    QVariantMap getUIConfig() { return m_cacheUIConfig; }
    void        setUIConfig(const QVariantMap& config);

    // pages
    QVariantMap getPages() { return m_cacheUIPages; }
    void        setPages(const QVariantMap& config);

    // groups
    QVariantMap getGroups() { return m_cacheUIGroups; }
    void        setGroups(const QVariantMap& config);

    // unit system
    UnitSystem getUnitSystem() override { return m_cacheUnitSystem; }
    void       setUnitSystem(UnitSystem value);

    // languages
    QVariantList getLanguages() { return m_languages; }

    // Shortcuts to get the config items, and to decouple a bit from Json structure
    // Please avoid old access via read property
    Q_INVOKABLE QVariantMap getProfile(const QString& profile) { return m_cacheUIProfiles.value(profile).toMap(); }
    Q_INVOKABLE QVariantMap getPage(const QString& pageId) { return m_cacheUIPages.value(pageId).toMap(); }
    Q_INVOKABLE QVariantMap getGroup(const QString& groupId) {
        return m_cacheUIGroups.value(groupId).toMap();
    }  // not used anywhere

    // The selected, cached profile
    Q_INVOKABLE QVariantMap getProfile() { return m_cacheUIProfile; }
    Q_INVOKABLE QStringList getProfilePages() { return m_cacheUIProfile.value("pages").toStringList(); }

    // set favorite entity
    void setFavorite(const QString& entityId, bool value);

    // read and write configuration to file
    bool readConfig(const QString& filePath);
    bool writeConfig();

    // get a QML object, you need to have objectName property of the QML object set to be able to use this
    QObject* getQMLObject(QList<QObject*> nodes, const QString& name);
    QObject* getQMLObject(const QString& name) override;

    // get all integrations and entities
    QVariantMap getAllIntegrations() override { return m_config["integrations"].toMap(); }
    QVariantMap getIntegration(const QString& type) override { return getAllIntegrations().value(type).toMap(); }

    QVariantMap  getAllEntities() override { return m_config["entities"].toMap(); }
    QVariantList getEntities(const QString& type) override { return getAllEntities().value(type).toList(); }

 public:
    explicit Config(QQmlApplicationEngine* engine, QString configFilePath, QString schemaFilePath, QString appPath);
    ~Config() override;

    static Config* getInstance() { return s_instance; }

 signals:
    void configChanged();
    void profileIdChanged();
    void profileFavoritesChanged();
    void settingsChanged();
    void profilesChanged();
    void uiConfigChanged();
    void pagesChanged();
    void groupsChanged();
    void unitSystemChanged();
    void configWriteError(const QString& error);

 private:
    void syncConfigToCache();
    void syncCacheToConfig();

    static Config*         s_instance;
    QQmlApplicationEngine* m_engine;

    QVariantMap  m_config;
    QVariantList m_languages;

    JsonFile* m_jsf;
    QString   m_error;

    JsonFile* m_tf;

    // Caches to improve performance
    QString     m_cacheProfileId;
    QVariantMap m_cacheSettings;
    QVariantMap m_cacheUIConfig;
    QVariantMap m_cacheUIProfile;  // of selected Profile
    QVariantMap m_cacheUIProfiles;
    QVariantMap m_cacheUIPages;
    QVariantMap m_cacheUIGroups;
    UnitSystem  m_cacheUnitSystem = METRIC;
};

typedef Config::UnitSystem UnitSystem;
