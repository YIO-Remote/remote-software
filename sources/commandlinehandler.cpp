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

#include "commandlinehandler.h"

#include <QCommandLineParser>

#include "config.h"
#include "jsonfile.h"
#include "logging.h"

CommandLineHandler::CommandLineHandler(QObject *parent) : QObject(parent), m_profile(""), m_cfgFile("") {}

void CommandLineHandler::process(const QCoreApplication &app, const QString &resourcePath,
                                 const QString &configurationPath) {
    QCommandLineParser parser;
    parser.setApplicationDescription("YIO remote app");
    parser.addHelpOption();
    parser.addVersionOption();
    // App options
    QCommandLineOption profileOption(QStringList() << "p"
                                                   << "profile",
                                     tr("Use configuration profile."), "NAME");
    QCommandLineOption cfgOpt(QStringList() << "c"
                                            << "cfg",
                              tr("Use configuration file."), "FILE");
    QCommandLineOption cfgSchemaOpt(QStringList() << "cfg-schema", tr("Use configuration schema file."), "FILE");
    QCommandLineOption hwCfgOpt(QStringList() << "hw-cfg", tr("Use hardware configuration file."), "FILE");
    QCommandLineOption hwCfgSchemaOpt(QStringList() << "hw-schema", tr("Use hardware configuration schema file."),
                                      "FILE");

    // Commands: execute & exit
    QCommandLineOption validateOption(QStringList() << "validate", tr("Validate json configuration files and exit."));

    parser.addOption(profileOption);

    parser.addOption(cfgOpt);
    parser.addOption(cfgSchemaOpt);
    parser.addOption(hwCfgOpt);
    parser.addOption(hwCfgSchemaOpt);

    parser.addOption(validateOption);

    parser.process(app);

    if (parser.isSet(profileOption)) {
        m_profile = parser.value(profileOption);
    }

    // Plain and simple profile option: apply to configuration files as suffix
    if (!m_profile.isEmpty()) {
        m_cfgFile = QString("%1/config-%2.json").arg(configurationPath).arg(m_profile);
        m_cfgSchemaFile = QString("%1/config-%2-schema.json").arg(resourcePath).arg(m_profile);
        m_hwCfgFile = QString("%1/hardware-%2.json").arg(resourcePath).arg(m_profile);
        m_hwCfgSchemaFile = QString("%1/hardware-%2-schema.json").arg(resourcePath).arg(m_profile);
    } else {
        m_cfgFile = QString("%1/%2").arg(configurationPath).arg(Config::CFG_FILE_NAME);
        m_cfgSchemaFile = QString("%1/config-schema.json").arg(resourcePath);
        m_hwCfgFile = QString("%1/hardware.json").arg(resourcePath);
        m_hwCfgSchemaFile = QString("%1/hardware-schema.json").arg(resourcePath);
    }

    // Check if a configuration file is specified as cmd line argument
    if (parser.isSet(cfgOpt)) {
        m_cfgFile = parser.value(cfgOpt);
    }
    if (parser.isSet(cfgSchemaOpt)) {
        m_cfgSchemaFile = parser.value(cfgSchemaOpt);
    }
    if (parser.isSet(hwCfgOpt)) {
        m_hwCfgFile = parser.value(hwCfgOpt);
    }
    if (parser.isSet(hwCfgSchemaOpt)) {
        m_hwCfgSchemaFile = parser.value(hwCfgSchemaOpt);
    }

    if (parser.isSet(validateOption)) {
        bool valid = validateJson(m_cfgFile, m_cfgSchemaFile);
        valid &= validateJson(m_hwCfgFile, m_hwCfgSchemaFile);
        qInfo() << (valid ? "OK" : "FAIL");  // no category logging: only used in cmd line

        exit(valid ? 0 : 1);
    }

    if (!QFile::exists(m_cfgSchemaFile)) {
        qCWarning(lcEnv)
            << "App configuration schema not found, configuration file will not be validated! Missing file:"
            << m_cfgSchemaFile;
    }

    if (!QFile::exists(m_hwCfgSchemaFile)) {
        qCWarning(lcEnv)
            << "Hardware configuration schema not found, configuration file will not be validated! Missing file:"
            << m_hwCfgSchemaFile;
    }
}

bool CommandLineHandler::validateJson(const QString &filePath, const QString &schemaPath) {
    JsonFile jsonFile(filePath, schemaPath);
    jsonFile.read();
    qInfo() << "Validating" << filePath << "with schema" << schemaPath;  // no category logging: only used in cmd line
    if (jsonFile.isValid()) {
        return true;
    } else {
        qCritical().noquote() << "Invalid configuration!" << endl << jsonFile.error();
        return false;
    }
}
