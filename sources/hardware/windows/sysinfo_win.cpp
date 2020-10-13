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

// Based on: https://github.com/PacktPublishing/Mastering-Qt-5/blob/master/Chapter_02/SysInfoWinImpl.cpp

#include "sysinfo_win.h"

#include <sysinfoapi.h>

#include "../../logging.h"

SystemInfoWin::SystemInfoWin(QObject* parent) : SystemInfo(parent) {}

void SystemInfoWin::init() {
    getCpuSample(&m_lastCpuSample);
    emit cpuTemperatureSupportChanged();
}

QString SystemInfoWin::uptime() {
    auto upSeconds = GetTickCount64() / 1000;
    int  days = static_cast<int>(upSeconds / 86400);
    int  hours = (upSeconds % 86400) / 3600;
    int  minutes = ((upSeconds % 86400) % 3600) / 60;

    return QString("%1 d %2 h %3 m").arg(days).arg(hours).arg(minutes);
}

float SystemInfoWin::usedMemory() {
    MEMORYSTATUSEX memoryStatus;
    memoryStatus.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memoryStatus);
    qulonglong memoryPhysicalUsed = memoryStatus.ullTotalPhys - memoryStatus.ullAvailPhys;
    return static_cast<float>(static_cast<double>(memoryPhysicalUsed) / static_cast<double>(memoryStatus.ullTotalPhys) *
                              100.0);
}

bool SystemInfoWin::cpuTemperatureSupported() {
    return false;
}

float SystemInfoWin::cpuTemperature() {
    // getting the cpu temp on Windows is a major pain...
    // This might be the way to go: https://docs.microsoft.com/en-us/windows/win32/cimwin32prov/win32-temperatureprobe
    // BUT: the whole WMI to CPP is a bit too much...
    return 0.0f;
}

float SystemInfoWin::cpuLoadAverage() {
    CpuSample sample, delta;
    if (!getCpuSample(&sample)) {
        return -1;
    }
    delta.totalSystemTime = sample.totalSystemTime - m_lastCpuSample.totalSystemTime;
    delta.totalUserTime = sample.totalUserTime - m_lastCpuSample.totalUserTime;
    delta.totalIdleTime = sample.totalIdleTime - m_lastCpuSample.totalIdleTime;
    auto totalSystemTime = delta.totalSystemTime + delta.totalUserTime;

    m_lastCpuSample = sample;

    auto percent = (totalSystemTime - delta.totalIdleTime) * 100.0 / totalSystemTime;
    return qBound(0.0f, static_cast<float>(percent), 100.0f);
}

bool SystemInfoWin::getCpuSample(SystemInfoWin::CpuSample* sample) {
    FILETIME kernelTime, userTime, idleTime;

    if (!GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        qCCritical(lcSysInfo) << "Error getting system time information";
        return false;
    }

    sample->totalSystemTime = convertFileTime(kernelTime);
    sample->totalUserTime = convertFileTime(userTime);
    sample->totalIdleTime = convertFileTime(idleTime);

    return true;
}

qulonglong SystemInfoWin::convertFileTime(const FILETIME& filetime) const {
    ULARGE_INTEGER largeInteger;
    largeInteger.LowPart = filetime.dwLowDateTime;
    largeInteger.HighPart = filetime.dwHighDateTime;
    return largeInteger.QuadPart;
}
