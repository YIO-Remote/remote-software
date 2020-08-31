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

#include <QObject>

enum class OS { Android, iOS, Linux, macOS, Windows, Other };

/**
 * @brief The Environment class provides utility functions to determine the runtime environment.
 */
class Environment : public QObject {
    Q_OBJECT

 public:
    static const char* ENV_YIO_HOME;
    static const char* ENV_YIO_APP_DIR;
    static const char* ENV_YIO_OS_VERSION;
    static const char* ENV_YIO_PLUGIN_DIR;

    static const QString UNKNOWN;

 public:
    /**
     * @brief Constructs and initialize an environment.
     * All getter methods will return the result without the overhead of the initialization.
     */
    explicit Environment(QObject* parent = nullptr);

    /**
     * @brief Returns the device type identifier for the software update function
     */
    QString getDeviceType() const { return m_deviceType; }

    /**
     * @brief Returns the operating system
     */
    OS getOperatingSystem() const { return m_os; }

    /**
     * @brief Returns true if the app runs on the YIO hardware remote
     */
    bool isYioRemote() const { return m_yioRemote; }

    /**
     * @brief Returns true if the app runs on a Raspberry Pi
     */
    bool isRaspberryPi() const { return m_raspberryPi; }

    /**
     * @brief Returns the YIO remote-os version.
     * @return The version string, e.g. "v0.2.0" or Environment::UNKNOWN if the app runs on another platform.
     */
    QString getRemoteOsVersion() const;

    /**
     * @brief Returns the directory which contains the application resources
     * @details The application resources are usually stored in the same directory of the application executable.
     */
    QString getResourcePath() const;
    QString getConfigurationPath() const;

    /**
     * @brief Sets the first run marker for the next startup.
     * @return True if the operation succeeded, false otherwise.
     */
    bool prepareFirstRun();

    /**
     * @brief Returns true if it's the first run of the remote to start the initial setup.
     */
    bool isFirstRun() const;

    /**
     * @brief Removes the first run marker.
     * @return True if the operation succeeded, false otherwise.
     */
    bool finishFirstRun();

 private:
    OS      determineOS();
    QString getRaspberryRevision(OS os);
    bool    runningOnYioRemote(const QString& rpiRevision);
    QString determineDeviceType(OS os, bool yioRemote, const QString& rpiRevision);

    QString m_deviceType;
    OS      m_os;
    bool    m_yioRemote;
    bool    m_raspberryPi;
    QString m_markerFileFirstRun;
};
