/******************************************************************************
 *
 * Copyright (C) 2020 Markus Zehnder <business@markuszehnder.ch>
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

#include <QCoreApplication>
#include <QObject>

class CommandLineHandler : public QObject {
    Q_OBJECT

 public:
    explicit CommandLineHandler(QObject *parent = nullptr);

    void process(const QCoreApplication &app, const QString &resourcePath, const QString &configurationPath);

    QString getProfile() const { return m_profile; }

    QString configFile() const { return m_cfgFile; }
    QString configSchemaFile() const { return m_cfgSchemaFile; }
    QString hardwareConfigFile() const { return m_hwCfgFile; }
    QString hardwareConfigSchemaFile() const { return m_hwCfgSchemaFile; }

 private:
    bool validateJson(const QString &filePath, const QString &schemaPath);

 private:
    QString m_profile;
    QString m_cfgFile;
    QString m_cfgSchemaFile;
    QString m_hwCfgFile;
    QString m_hwCfgSchemaFile;
};
