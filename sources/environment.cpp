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

#include "environment.h"

#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QStorageInfo>
#include <QSysInfo>
#include <QTextStream>

#include "logging.h"

const char *Environment::ENV_YIO_HOME = "YIO_HOME";
const char *Environment::ENV_YIO_APP_DIR = "YIO_APP_DIR";
const char *Environment::ENV_YIO_OS_VERSION = "YIO_OS_VERSION";
const char *Environment::ENV_YIO_PLUGIN_DIR = "YIO_PLUGIN_DIR";

const QString Environment::UNKNOWN = "UNKNOWN";

Environment::Environment(QObject *parent) : QObject(parent), m_markerFileFirstRun("/firstrun") {
    m_os = determineOS();
    QString rpiRevision = getRaspberryRevision(m_os);
    m_raspberryPi = !rpiRevision.isEmpty();
    m_yioRemote = runningOnYioRemote(rpiRevision);
    m_deviceType = determineDeviceType(m_os, m_yioRemote, rpiRevision);

    if (m_yioRemote) {
        QStorageInfo storage = QStorageInfo("/");
        if (storage.isReadOnly()) {
            m_markerFileFirstRun = "/var/yio/firstrun";
        }
    }

    qCDebug(lcEnv) << "Device type:" << m_deviceType << "RPi revision:" << rpiRevision << "YIO remote:" << m_yioRemote
                   << "First run marker file:" << m_markerFileFirstRun;
}

QString Environment::getRemoteOsVersion() const {
    return qEnvironmentVariable(ENV_YIO_OS_VERSION, UNKNOWN);
}

QString Environment::getResourcePath() const {
    QString appPath = QCoreApplication::applicationDirPath();
    if (m_os == OS::macOS) {
        return QFileInfo(appPath + "/../").canonicalPath() + "/Contents/Resources";
    }

    return appPath;
}

QString Environment::getConfigurationPath() const {
    if (m_yioRemote) {
        return "/boot";
    }

    return getResourcePath();
}

QString Environment::getUserStoragePath() const {
    if (m_yioRemote) {
        return "/var/yio";
    }

    // TODO(zehnm) refine storage path for non YIO remote environments
    return getResourcePath() + "/UserData";
}

bool Environment::prepareFirstRun() {
    qCDebug(lcEnv) << "Creating marker file for first run:" << m_markerFileFirstRun;

    m_error.clear();

    QFile file(m_markerFileFirstRun);
    if (!file.open(QIODevice::WriteOnly)) {
        m_error = "Error writing firstrun marker file: " + m_markerFileFirstRun;
        qCCritical(lcEnv) << m_error;
        return false;
    }
    file.close();
    return true;
}

bool Environment::isFirstRun() const {
    bool firstRun = QFile::exists(m_markerFileFirstRun);
    qCDebug(lcEnv) << "First run:" << firstRun;
    return firstRun;
}

bool Environment::finishFirstRun() {
    qCDebug(lcEnv) << "First run finished, removing marker file:" << m_markerFileFirstRun;
    return QFile::remove(m_markerFileFirstRun);
}

OS Environment::determineOS() {
#if defined(Q_OS_ANDROID)
    return OS::Android;
#elif defined(Q_OS_IOS)
    return OS::iOS;
#elif defined(Q_OS_LINUX)
    return OS::Linux;
#elif defined(Q_OS_WIN64)
    return OS::Windows;
#elif defined(Q_OS_WIN32)
    return OS::Windows;
#elif defined(Q_OS_MACOS)
    return OS::macOS;
#else
    return OS::Other;
#endif
}

QString Environment::getRaspberryRevision(OS os) {
#if !defined(Q_PROCESSOR_ARM)
    return QString();
#endif

    if (os != OS::Linux) {
        return QString();
    }

    QString hardware, revision;
    QFile   cpuinfo("/proc/cpuinfo");
    if (cpuinfo.open(QIODevice::ReadOnly)) {
        QTextStream in(&cpuinfo);
        QString     line;
        while (in.readLineInto(&line)) {
            if (line.startsWith("Hardware\t:")) {
                hardware = line.mid(line.indexOf(':') + 1).trimmed();
            } else if (line.startsWith("Revision\t:")) {
                revision = line.mid(line.indexOf(':') + 1).trimmed();
            }
        }
        cpuinfo.close();
    } else {
        qCDebug(lcEnv) << "Error opening /proc/cpuinfo:" << cpuinfo.errorString();
    }

    qCDebug(lcEnv) << "cpuinfo: hardware=" << hardware << "revision=" << revision;

    // TODO(zehnm) is this really required? Or is RPi always BCM2835? Looks like a kernel thing...
    if (hardware.contains(QRegExp("(BCM2835|BCM2836|BCM2837|BCM2838|BCM2711|BCM2708|BCM2709)"))) {
        return revision;
    }

    return QString();
}

bool Environment::runningOnYioRemote(const QString &rpiRevision) {
    if (qEnvironmentVariableIsSet("I_AM_YIO")) {
        qCInfo(lcEnv) << "Magic env variable set: we are now a YIO remote!";
        return true;
    }

    bool envVarsPresent = qEnvironmentVariableIsSet(ENV_YIO_HOME) && qEnvironmentVariableIsSet(ENV_YIO_APP_DIR) &&
                          qEnvironmentVariableIsSet(ENV_YIO_OS_VERSION);

    if (rpiRevision.isEmpty()) {
        return false;
    }

    // RPi revision code: https://www.raspberrypi-spy.co.uk/2012/09/checking-your-raspberry-pi-board-version/
    return rpiRevision.compare("9000c1", Qt::CaseInsensitive) == 0 && envVarsPresent;
}

QString Environment::determineDeviceType(OS os, bool yioRemote, const QString &rpiRevision) {
    if (yioRemote) {
        return "remote";
    }

    if (!rpiRevision.isEmpty()) {
        return QString("rpi:%1").arg(rpiRevision);
    }

    QString cpu = QSysInfo::currentCpuArchitecture();

    switch (os) {
        case OS::Android:
            return QString("android:%1").arg(cpu);
        case OS::iOS:
            return "ios";
        case OS::Linux:
            return QString("linux:%1").arg(cpu);
        case OS::macOS:
            return QString("mac:%1").arg(cpu);
        case OS::Windows:
            return QString("windows:%1").arg(cpu);
        default:
            return QString("other:%1").arg(cpu);
    }
}
