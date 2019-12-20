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

Config::Config(QQmlApplicationEngine *engine, QString path) :
    m_engine(engine)
{
    m_jsf = new JsonFile();
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
    QString p = profile();
    QStringList fav = getProfileFavorites();
    if (value && !fav.contains(entityId))
        fav.append(entityId);

    if (!value && fav.contains(entityId))
        fav.removeOne(entityId);

    // update config
    QVariantMap profile = getProfile();
    profile.insert("favorites", fav);

    QVariantMap profiles = getUIConfig().value("profiles").toMap();
    profiles.insert(p, profile);

    QVariantMap uiConfig = getUIConfig();
    uiConfig.insert("profiles", profiles);

    m_config.insert("ui_config", uiConfig);

    writeConfig();

    emit configChanged();
}


void Config::readConfig(QString path)
{
    // load the config.json file from the filesystem
    m_jsf->setName(path + "/config.json");
    m_config = m_jsf->read().toMap();
    syncCache();

    emit configChanged();
}

void Config::writeConfig()
{
    syncCache();                    // After every config change writeConfig will be called
    m_jsf->write(m_config);
}

void Config::writeConfig(const bool sync)
{
    if (sync) {
        syncCache();
        m_jsf->write(m_config);
    } else {
        m_jsf->write(m_config);
    }
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


void Config::syncCache ()
{
    m_cacheSettings     = m_config["settings"].toMap();
    emit settingsChanged();
    m_cacheUIConfig     = m_config["ui_config"].toMap();
    emit uiConfigChanged();
    m_cacheProfile      = m_cacheUIConfig["selected_profile"].toString();
    m_cacheUIProfiles   = m_cacheUIConfig["profiles"].toMap();
    emit profilesChanged();
    m_cacheUIProfile    = m_cacheUIProfiles[m_cacheProfile].toMap();
    emit profileChanged();
    m_cacheUIPages      = m_cacheUIConfig["pages"].toMap();
    emit pagesChanged();
    m_cacheUIGroups     = m_cacheUIConfig["groups"].toMap();
    emit groupsChanged();
}
