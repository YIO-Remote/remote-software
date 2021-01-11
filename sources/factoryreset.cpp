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

#include "factoryreset.h"

#include "config.h"
#include "hardware/hardwarefactory.h"

FactoryReset::FactoryReset(Environment* environment, QObject* parent) : QObject(parent), m_environment(environment) {}

bool FactoryReset::isSupported() const { return m_environment->isYioRemote(); }

void FactoryReset::performReset() {
    if (!isSupported()) {
        emit factoryResetFailed("Not supported on this device.");
        return;
    }
    emit factoryResetStarted();

    Config*          config = Config::getInstance();
    HardwareFactory* hwFactory = HardwareFactory::instance();
    QVariantMap      oldCfg = config->getConfig();

    if (config->resetConfigurationForFirstRun()) {
        // reset wifi settings
        hwFactory->getWifiControl()->clearConfiguredNetworks();
        // TODO(zehnm) further clean up like puring log files and user storage
        emit factoryResetCompleted();
    } else {
        QString error = config->getError();
        config->setConfig(oldCfg);
        emit factoryResetFailed(error);
    }
}
