/******************************************************************************
 *
 * Copyright (C) 2019 Markus Zehnder <business@markuszehnder.ch>
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

#ifndef HARDWAREFACTORY_H
#define HARDWAREFACTORY_H

#include <QObject>

#include "systemservice.h"
#include "webserver_control.h"
#include "wifi_control.h"

/**
 * @brief Abstract hardware factory. Supported platforms are implemented in concrete factories.
 */
class HardwareFactory : public QObject
{
    Q_OBJECT
public:

    /**
     * @brief build Constructs the singleton hardware factory with a concrete implementation of the factory.
     * @details One of the build operations must be called once before using the singleton instance() accessor.
     * @param configFileName the JSON configuration file name to load the hardware configuration from.
     * @param schemaFileName the JSON schema file to validate the configuration file.
     * @return The concrete instance or nullptr if construction failed. In this case the application should be terminated.
     */
    static HardwareFactory* build(const QString &configFileName, const QString &schemaFileName);

    /**
     * @brief build Constructs the singleton hardware factory with a concrete implementation of the factory.
     * @details One of the build operations must be called once before using the singleton instance() accessor.
     * @param config The configuration map to retrieve build configuration options.
     * @return The concrete instance or nullptr if construction failed. In this case the application should be terminated.
     */
    static HardwareFactory* build(const QVariantMap &config);

    /**
     * @brief instance Returns a concrete HardwareFactory implementation
     * @return The hardware factory or nullptr if the factory has not yet been initialized
     */
    static HardwareFactory* instance();

    virtual WifiControl* getWifiControl() = 0;

    virtual SystemService* getSystemService() = 0;

    virtual WebServerControl* getWebServerControl() = 0;

signals:

protected:
    HardwareFactory(QObject *parent = nullptr);
    virtual ~HardwareFactory();

private:
    static HardwareFactory* s_instance;
};

#endif // HARDWAREFACTORY_H
