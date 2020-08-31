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

#include "config.h"

#include <QJsonDocument>
#include <QLoggingCategory>

#if defined(Q_OS_LINUX)
#include <filesystem>
#endif

#include "environment.h"

static Q_LOGGING_CATEGORY(CLASS_LC, "config");

const QString Config::CFG_FILE_NAME = "config.json";
const QString Config::CFG_FILE_NAME_DEF = "config.json.def";
const QString Config::KEY_ID = CFG_KEY_ID;
const QString Config::KEY_FRIENDLYNAME = CFG_KEY_FRIENDLYNAME;
const QString Config::KEY_ENTITY_ID = CFG_KEY_ENTITY_ID;
const QString Config::KEY_AREA = CFG_KEY_AREA;
const QString Config::KEY_INTEGRATION = CFG_KEY_INTEGRATION;
const QString Config::KEY_SUPPORTED_FEATURES = CFG_KEY_SUPPORTED_FEATURES;
const QString Config::KEY_TYPE = CFG_KEY_TYPE;
const QString Config::KEY_MDNS = CFG_KEY_MDNS;
const QString Config::KEY_WORKERTHREAD = CFG_KEY_WORKERTHREAD;
const QString Config::OBJ_DATA = CFG_OBJ_DATA;

Config *Config::s_instance = nullptr;

ConfigInterface::~ConfigInterface() {}

Config::Config(QQmlApplicationEngine *engine, QString configFilePath, QString schemaFilePath, Environment *environment)
    : m_engine(engine), m_jsf(configFilePath, schemaFilePath), m_error(""), m_environment(environment) {
    Q_ASSERT(engine);
    Q_ASSERT(environment);

    s_instance = this;

    // load translations file
    JsonFile translationCfg(environment->getResourcePath().append(QString("/translations.json")), "");
    m_languages = translationCfg.read().toList();

    qCDebug(CLASS_LC) << "Configuration file:" << configFilePath;
}

Config::~Config() { s_instance = nullptr; }

void Config::setFavorite(const QString &entityId, bool value) {
    QStringList fav = profileFavorites();

    if (value && !fav.contains(entityId)) {
        fav.append(entityId);
    }

    if (!value && fav.contains(entityId)) {
        fav.removeOne(entityId);
    }

    m_cacheUIProfile.insert("favorites", fav);
    writeConfig();
    emit profileFavoritesChanged();
}

void Config::setConfig(const QVariantMap &config) {
    m_error.clear();
    if (!m_jsf.validate(QJsonDocument::fromVariant(config), m_error)) {
        return;
    }

    m_config = config;
    syncConfigToCache();
    emit configChanged();
    writeConfig();
}

bool Config::readConfig() {
    // load the config.json file from the filesystem
    m_config = m_jsf.read().toMap();
    m_error = m_jsf.error();
    syncConfigToCache();
    emit configChanged();

    return m_jsf.isValid();
}

bool Config::writeConfig() {
    syncCacheToConfig();
    bool result = m_jsf.write(m_config);
    m_error = m_jsf.error();
    if (!result) {
        emit configWriteError(m_error);
    }
    return result;
}

void Config::setSettings(const QVariantMap &config) {
    m_cacheSettings = config;
    writeConfig();
    emit settingsChanged();
}

void Config::setProfiles(const QVariantMap &config) {
    m_cacheUIProfiles = config;
    m_cacheUIProfile = m_cacheUIProfiles[m_cacheProfileId].toMap();
    writeConfig();
    emit profilesChanged();
}

void Config::setUIConfig(const QVariantMap &config) {
    m_cacheUIConfig = config;
    writeConfig();
    emit uiConfigChanged();
}

void Config::setPages(const QVariantMap &config) {
    m_cacheUIPages = config;
    writeConfig();
    emit pagesChanged();
}

void Config::setGroups(const QVariantMap &config) {
    m_cacheUIGroups = config;
    writeConfig();
    emit groupsChanged();
}

void Config::setUnitSystem(UnitSystem::Enum value) {
    if (m_cacheUnitSystem != value) {
        m_cacheUnitSystem = value;
        emit unitSystemChanged();
    }
    writeConfig();
}

QObject *Config::getQMLObject(QList<QObject *> nodes, const QString &name) {
    for (int i = 0; i < nodes.size(); i++) {
        if (nodes.at(i) && nodes.at(i)->objectName() == name) {
            return dynamic_cast<QObject *>(nodes.at(i));
        } else if (nodes.at(i) && nodes.at(i)->children().size() > 0) {
            QObject *item = getQMLObject(nodes.at(i)->children(), name);
            if (item) {
                return item;
            }
        }
    }
    return nullptr;
}

QObject *Config::getQMLObject(const QString &name) { return getQMLObject(m_engine->rootObjects(), name); }

bool Config::resetConfigurationForFirstRun() {
    qCWarning(CLASS_LC) << "Resetting remote configuration for first run";

    QString defaultConfigFile =
        qEnvironmentVariable(Environment::ENV_YIO_APP_DIR, m_environment->getResourcePath()) + "/" + CFG_FILE_NAME_DEF;

    if (!QFile::exists(defaultConfigFile)) {
        qCCritical(CLASS_LC) << "Default config file not found. Cannot restore to defaults.";
        return false;
    }

    if (m_environment->isYioRemote()) {
        QString cfgFile = QString("%1/%2").arg(m_environment->getConfigurationPath()).arg(CFG_FILE_NAME);
        QString cfgFileBackup = cfgFile + ".old";
        qCDebug(CLASS_LC) << "Backing up configuration file" << cfgFile
                          << "and replacing it with default configuration:" << defaultConfigFile;
#if defined(Q_OS_LINUX)
        try {
            // make a copy of existing configuration
            if (!std::filesystem::copy_file(cfgFile.toStdString(), cfgFileBackup.toStdString(),
                                            std::filesystem::copy_options::overwrite_existing)) {
                qCCritical(CLASS_LC) << "Error backing up existing configuration.";
                return false;
            }

            if (!std::filesystem::copy_file(defaultConfigFile.toStdString(), cfgFile.toStdString(),
                                            std::filesystem::copy_options::overwrite_existing)) {
                qCCritical(CLASS_LC) << "Error copying default configuration.";
                return false;
            }
        } catch (std::exception &e) {
            qCCritical(CLASS_LC) << "Error resetting configuration:" << qPrintable(e.what());
            return false;
        }
#endif
    }

    return m_environment->prepareFirstRun();
}

void Config::setProfileId(QString id) {
    qCDebug(CLASS_LC) << "Profile id changing to:" << id << "from:" << m_cacheProfileId;
    m_cacheProfileId = id;
    m_cacheUIProfile = m_cacheUIProfiles[m_cacheProfileId].toMap();

    writeConfig();
    emit profileIdChanged();
}

void Config::syncConfigToCache() {
    m_cacheSettings = m_config["settings"].toMap();
    m_cacheUIConfig = m_config["ui_config"].toMap();

    m_cacheProfileId = m_cacheUIConfig["selected_profile"].toString();
    m_cacheUIProfiles = m_cacheUIConfig["profiles"].toMap();
    m_cacheUIPages = m_cacheUIConfig["pages"].toMap();
    m_cacheUIGroups = m_cacheUIConfig["groups"].toMap();

    m_cacheUIProfile = m_cacheUIProfiles[m_cacheProfileId].toMap();

    m_cacheUnitSystem = stringToEnum<UnitSystem::Enum>(m_cacheSettings["unit"], UnitSystem::METRIC);
}

void Config::syncCacheToConfig() {
    m_cacheUIConfig.insert("selected_profile", m_cacheProfileId);
    m_cacheUIConfig.insert("profiles", m_cacheUIProfiles);
    m_cacheUIConfig.insert("pages", m_cacheUIPages);
    m_cacheUIConfig.insert("groups", m_cacheUIGroups);

    m_cacheSettings.insert("unit", enumToString(m_cacheUnitSystem));

    m_config.insert("settings", m_cacheSettings);
    m_config.insert("ui_config", m_cacheUIConfig);
}
