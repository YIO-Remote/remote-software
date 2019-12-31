/******************************************************************************
 *
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

#include "configinterface.h"
#include "jsonfile.h"

class Config : public QObject, ConfigInterface {
    Q_OBJECT
    Q_INTERFACES(ConfigInterface)

 public:
    Q_PROPERTY(bool valid READ isValid CONSTANT)
    Q_PROPERTY(QString error READ getError CONSTANT)
    Q_PROPERTY(QVariantMap config READ config WRITE setConfig NOTIFY configChanged)
    Q_PROPERTY(QString profile READ profile WRITE setProfile NOTIFY profileChanged)
    Q_PROPERTY(QStringList profileFavorites READ profileFavorites NOTIFY profileFavoritesChanged)
    Q_PROPERTY(QVariantMap settings READ getSettings WRITE setSettings NOTIFY settingsChanged)
    Q_PROPERTY(QVariantMap integrations READ getIntegrations CONSTANT)
    Q_PROPERTY(QVariantMap entities READ getAllEntities CONSTANT)
    Q_PROPERTY(QVariantMap profiles READ getProfiles NOTIFY profilesChanged)
    Q_PROPERTY(QVariantMap ui_config READ getUIConfig WRITE setUIConfig NOTIFY uiConfigChanged)
    Q_PROPERTY(QVariantMap pages READ getPages NOTIFY pagesChanged)
    Q_PROPERTY(QVariantMap groups READ getGroups NOTIFY groupsChanged)

    Q_INVOKABLE bool readConfig(const QString& path);
    Q_INVOKABLE bool writeConfig();

    // Shortcuts to get the config items, and to decouple a bit from Json structure
    // Please avoid old access via read property
    QVariantMap              getIntegrations() { return m_config["integrations"].toMap(); }
    Q_INVOKABLE QVariantMap  getIntegration(const QString& type) { return getIntegrations().value(type).toMap(); }
    QVariantMap              getAllEntities() { return m_config["entities"].toMap(); }
    Q_INVOKABLE QVariantList getEntities(const QString& type) { return getAllEntities().value(type).toList(); }

    // Assuming that the following config items are accessed quite often they are cached
    QVariantMap getSettings() { return m_cacheSettings; }

    void setSettings(const QVariantMap& config);

    QVariantMap getUIConfig() { return m_cacheUIConfig; }
    void        setUIConfig(const QVariantMap& config);

    QVariantMap             getProfiles() { return m_cacheUIProfiles; }
    Q_INVOKABLE QVariantMap getProfile(const QString& profile) { return m_cacheUIProfiles.value(profile).toMap(); }
    QVariantMap             getPages() { return m_cacheUIPages; }
    Q_INVOKABLE QVariantMap getPage(const QString& pageId) { return m_cacheUIPages.value(pageId).toMap(); }
    QVariantMap             getGroups() { return m_cacheUIGroups; }
    Q_INVOKABLE QVariantMap getGroup(const QString& groupId) { return m_cacheUIGroups.value(groupId).toMap(); }

    // The selected, cached profile
    Q_INVOKABLE QVariantMap getProfile() { return m_cacheUIProfile; }
    Q_INVOKABLE QStringList profileFavorites() { return m_cacheUIProfile.value("favorites").toStringList(); }
    Q_INVOKABLE QStringList getProfilePages() { return m_cacheUIProfile.value("pages").toStringList(); }

    // Removed from entities
    Q_INVOKABLE void setFavorite(const QString& entityId, bool value);

    bool    isValid() const { return m_error.isEmpty(); }
    QString getError() const { return m_error; }

    QVariantMap config() { return m_config; }
    void        setConfig(const QVariantMap& config);

    QVariant getContextProperty(const QString& name);

    // get a QML object, you need to have objectName property of the QML object set to be able to use this
    QObject* getQMLObject(QList<QObject*> nodes, const QString& name);
    QObject* getQMLObject(const QString& name);

    // profile
    QString profile() { return m_cacheProfile; }
    void    setProfile(QString id);

 public:
    explicit Config(QQmlApplicationEngine* engine = nullptr, QString path = "", QString schemaPath = "");
    virtual ~Config();

    static Config* getInstance() { return s_instance; }

 signals:
    void configChanged();
    void profileChanged();
    void profileFavoritesChanged();
    void settingsChanged();
    void profilesChanged();
    void uiConfigChanged();
    void pagesChanged();
    void groupsChanged();

 private:
    void syncConfigToCache();
    void syncCacheToConfig();

    static Config*         s_instance;
    QQmlApplicationEngine* m_engine;

    QVariantMap m_config;

    JsonFile* m_jsf;
    QString   m_error;

    // Caches to improve performance
    QString     m_cacheProfile;
    QVariantMap m_cacheSettings;
    QVariantMap m_cacheUIConfig;
    QVariantMap m_cacheUIProfile;  // of selected Profile
    QVariantMap m_cacheUIProfiles;
    QVariantMap m_cacheUIPages;
    QVariantMap m_cacheUIGroups;
};
