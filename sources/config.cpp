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

#include "config.h"

ConfigInterface::~ConfigInterface()
{}
Config* Config::s_instance = nullptr;

Config::Config(QQmlApplicationEngine *engine, QString path, QString schemaPath)
    : m_engine(engine)
{
    m_jsf = new JsonFile();
    m_jsf->setSchemaPath(schemaPath);
    s_instance = this;

    //load the config file
    readConfig(path);
}

Config::~Config()
{
    s_instance = nullptr;
}

void Config::setFavorite (const QString& entityId, bool value)
{
    QStringList fav = profileFavorites();

    if (value && !fav.contains(entityId))
        fav.append(entityId);

    if (!value && fav.contains(entityId))
        fav.removeOne(entityId);

    m_cacheUIProfile.insert("favorites", fav);
    writeConfig();
    emit profileFavoritesChanged();
}

void Config::setConfig(const QVariantMap &config)
{
    // TODO validate configuration!
    m_config = config;
    syncConfigToCache();
    emit configChanged();
}

QVariant Config::getContextProperty(const QString &name)
{
    return m_engine->rootContext()->contextProperty(name);
}


bool Config::readConfig(const QString &path)
{
    // load the config.json file from the filesystem
    m_jsf->setName(path + "/config.json");
    m_config = m_jsf->read().toMap();
    m_valid = m_jsf->isValid();
    syncConfigToCache();
    emit configChanged();

    return m_valid;
}

bool Config::writeConfig()
{
    syncCacheToConfig();
    return m_jsf->write(m_config);
}

void Config::setSettings(const QVariantMap &config) {
    m_cacheSettings = config;
    emit settingsChanged();
    writeConfig();
}

void Config::setUIConfig(const QVariantMap &config)
{
    m_cacheUIConfig = config;
    emit uiConfigChanged();
    writeConfig();
}

QObject *Config::getQMLObject(QList<QObject*> nodes,const QString &name)
{
    for (int i=0; i < nodes.size(); i++)
    {
        if (nodes.at(i) && nodes.at(i)->objectName() == name) {
            return dynamic_cast<QObject*>(nodes.at(i));
        } else if (nodes.at(i) && nodes.at(i)->children().size() > 0) {
            QObject* item = getQMLObject(nodes.at(i)->children(), name);
            if (item)
                return item;
        }
    }
    return  nullptr;
}

QObject *Config::getQMLObject(const QString &name)
{
    return getQMLObject(m_engine->rootObjects(), name);
}

void Config::setProfile(QString id)
{
    QVariantMap p = getUIConfig();
    p.insert("selected_profile", id);
    m_config.insert("ui_config", p);

    writeConfig();
    emit profileChanged();
}


void Config::syncConfigToCache()
{
    m_cacheSettings     = m_config["settings"].toMap();
    m_cacheUIConfig     = m_config["ui_config"].toMap();

    m_cacheProfile      = m_cacheUIConfig["selected_profile"].toString();
    m_cacheUIProfiles   = m_cacheUIConfig["profiles"].toMap();
    m_cacheUIPages      = m_cacheUIConfig["pages"].toMap();
    m_cacheUIGroups     = m_cacheUIConfig["groups"].toMap();

    m_cacheUIProfile    = m_cacheUIProfiles[m_cacheProfile].toMap();
}

void Config::syncCacheToConfig()
{
    m_cacheUIProfiles.insert(m_cacheProfile, m_cacheUIProfile);

    m_cacheUIConfig.insert("selected_profile", m_cacheProfile);
    m_cacheUIConfig.insert("profiles", m_cacheUIProfiles);
    m_cacheUIConfig.insert("pages", m_cacheUIPages);
    m_cacheUIConfig.insert("groups", m_cacheUIGroups);

    m_config.insert("settings", m_cacheSettings);
    m_config.insert("ui_config", m_cacheUIConfig);
}
