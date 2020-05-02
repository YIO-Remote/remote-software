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

#include "launcher.h"

#include <QLoggingCategory>
#include <QPluginLoader>
#include <QtDebug>

#include "notifications.h"

static Q_LOGGING_CATEGORY(CLASS_LC, "plugin");

Launcher::Launcher(QObject *parent) : QObject(parent), m_process(new QProcess(this)) {}

QString Launcher::launch(const QString &program) {
    m_process->start(program);
    m_process->waitForFinished(-1);
    QByteArray bytes  = m_process->readAllStandardOutput();
    QString    output = QString::fromLocal8Bit(bytes);
    return output;
}

QObject *Launcher::loadPlugin(const QString &path, const QString &pluginName) {
    QString       pluginPath = getPluginPath(path, pluginName);
    QPluginLoader pluginLoader(pluginPath, this);

    QJsonObject metaData = pluginLoader.metaData()["MetaData"].toObject();
    qCInfo(CLASS_LC) << "LOADING PLUGIN:" << pluginPath << "version:" << metaData["version"].toString();

    QObject *plugin = pluginLoader.instance();
    if (!plugin) {
        qCCritical(CLASS_LC) << "FAILED TO LOAD PLUGIN: " << pluginPath << pluginLoader.errorString();
        Notifications::getInstance()->add(true, "Failed to load " + QString(pluginName));
    }

    return plugin;
}

QString Launcher::getPluginPath(const QString &path, const QString &pluginName) {
    QString pluginPath;
#ifdef __arm__
    pluginPath = path + "/lib" + pluginName;
#elif __APPLE__
    pluginPath = path + "/lib" + pluginName + ".dylib";
#elif _WIN32
    pluginPath = path + "/" + pluginName + ".dll";
#else
    pluginPath = path + "/lib" + pluginName;
#endif
    return pluginPath;
}
