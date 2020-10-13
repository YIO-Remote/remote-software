/******************************************************************************
 *
 * Copyright (C) 2020 Markus Zehnder <business@markuszehnder.ch>
 *
 * Memory and CPU load code from "Mastering Qt 5"
 * Copyright (c) 2016 Packt - MIT License
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

// Based on: https://github.com/PacktPublishing/Mastering-Qt-5/blob/master/Chapter_02/SysInfoLinuxImpl.cpp

#include "sysinfo_linux.h"

#include <sys/sysinfo.h>
#include <sys/types.h>

#include <QFile>

#include "../../logging.h"

SystemInfoLinux::SystemInfoLinux(const QString &sysfsTemp, QObject *parent)
    : SystemInfo(parent), m_sysfsTemp(sysfsTemp), m_lastCpuSample({0, 0, 0}) {}

void SystemInfoLinux::init() {
    getCpuSample(&m_lastCpuSample);
}

QString SystemInfoLinux::uptime() {
    struct sysinfo sys_info;

    if (sysinfo(&sys_info) != 0) {
        qCWarning(lcSysInfo) << "Failed to call sysinfo:" << strerror(errno);
        return "error";
    }

    int days = sys_info.uptime / 86400;
    int hours = (sys_info.uptime / 3600) - (days * 24);
    int minutes = (sys_info.uptime / 60) - (days * 1440) - (hours * 60);

    return QString("%1 d %2 h %3 m").arg(days).arg(hours).arg(minutes);
}

float SystemInfoLinux::usedMemory() {
    struct sysinfo memInfo;
    if (sysinfo(&memInfo) != 0) {
        return 0;
    }

    qulonglong totalMemory = memInfo.totalram;
    // unfortunately sysinfo() doesn't return cache usage, therefore memory usage is (much) higher!
    qulonglong totalMemoryUsed = totalMemory - memInfo.freeram - memInfo.bufferram;

    float percent = static_cast<float>(static_cast<double>(totalMemoryUsed) / static_cast<double>(totalMemory) * 100.0);
    return qBound(0.0f, percent, 100.0f);
}

bool SystemInfoLinux::cpuTemperatureSupported() {
    return m_cpuTempSupported;
}

float SystemInfoLinux::cpuTemperature() {
    QFile file(m_sysfsTemp);
    if (!file.open(QIODevice::ReadOnly)) {
        if (m_cpuTempSupported) {
            qCWarning(lcSysInfo) << "Temperature cannot be read from sysfs:" << m_sysfsTemp
                                 << "Error:" << file.errorString();
            m_cpuTempSupported = false;
            emit cpuTemperatureSupportChanged();
        }
        return 0;
    }

    QByteArray line = file.readLine();
    file.close();

    bool ok;
    int  temp = line.toInt(&ok);
    if (!ok || temp < 1) {
        return 0;
    }

    return temp / 1000;
}

float SystemInfoLinux::cpuLoadAverage() {
    CpuSample sample, delta;
    if (!getCpuSample(&sample)) {
        return -1;
    }
    delta.totalSystemTime = sample.totalSystemTime - m_lastCpuSample.totalSystemTime;
    delta.totalUserTime = sample.totalUserTime - m_lastCpuSample.totalUserTime;
    delta.totalIdleTime = sample.totalIdleTime - m_lastCpuSample.totalIdleTime;

    m_lastCpuSample = sample;

    auto totalTime = delta.totalSystemTime + delta.totalUserTime + delta.totalIdleTime;
    auto onePercent = totalTime / 100.0;
    if (lcSysInfo().isDebugEnabled()) {
        qCDebug(lcSysInfo) << "system:" << static_cast<double>(delta.totalSystemTime) / onePercent
                           << "user:" << static_cast<double>(delta.totalUserTime) / onePercent
                           << "idle:" << static_cast<double>(delta.totalIdleTime) / onePercent;
    }

    auto percent = static_cast<double>(delta.totalSystemTime + delta.totalUserTime) / onePercent;
    return qBound(0.0f, static_cast<float>(percent), 100.0f);
}

bool SystemInfoLinux::getCpuSample(SystemInfoLinux::CpuSample *sample) {
    QFile file("/proc/stat");
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QByteArray line = file.readLine();
    file.close();
    quint64 totalUser = 0, totalUserNice = 0, totalSystem = 0, totalIdle = 0;
    int ret = std::sscanf(line.data(), "cpu %llu %llu %llu %llu", &totalUser, &totalUserNice, &totalSystem, &totalIdle);
    if (ret != 4) {
        qCWarning(lcSysInfo) << "sscanf failed";
        return false;
    }

    sample->totalSystemTime = totalSystem;
    sample->totalUserTime = totalUser + totalUserNice;
    sample->totalIdleTime = totalIdle;

    return true;
}
