/******************************************************************************
 *
 * Copyright (C) 2020 Markus Zehnder <business@markuszehnder.ch>
 *
 * Memory code from "Mastering Qt 5"
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

// Based on: https://github.com/PacktPublishing/Mastering-Qt-5/blob/master/Chapter_02/SysInfoMacImpl.cpp

#include "sysinfo_mac.h"

#include <errno.h>
#include <mach/mach_error.h>
#include <mach/mach_host.h>
#include <mach/mach_init.h>
#include <mach/mach_types.h>
#include <mach/vm_map.h>
#include <mach/vm_statistics.h>
#include <sys/sysctl.h>
#include <time.h>

#include "../../logging.h"
#include "smc.h"

SystemInfoMac::SystemInfoMac(QObject *parent) : SystemInfo(parent) {}

void SystemInfoMac::init() {
    getCpuSample(&m_lastCpuSample);
}

QString SystemInfoMac::uptime() {
    struct timeval boottime;
    size_t         len = sizeof(boottime);
    int            mib[2] = {CTL_KERN, KERN_BOOTTIME};

    if (sysctl(mib, 2, &boottime, &len, NULL, 0) < 0) {
        qCCritical(lcSysInfo) << "Failed to read kernel boot time";
        return "error";
    }

    time_t upSeconds = difftime(time(NULL), boottime.tv_sec);
    int    days = upSeconds / 86400;
    int    hours = (upSeconds % 86400) / 3600;
    int    minutes = ((upSeconds % 86400) % 3600) / 60;

    return QString("%1 d %2 h %3 m").arg(days).arg(hours).arg(minutes);
}

float SystemInfoMac::usedMemory() {
    vm_size_t              pageSize;
    vm_statistics64_data_t vmStats;
    mach_port_t            machPort = mach_host_self();
    mach_msg_type_number_t count = sizeof(vmStats) / sizeof(natural_t);

    host_page_size(machPort, &pageSize);
    host_statistics64(machPort, HOST_VM_INFO, (host_info64_t)&vmStats, &count);

    qlonglong freeMemory = (int64_t)vmStats.free_count * (int64_t)pageSize;

    qlonglong totalMemoryUsed =
        ((int64_t)vmStats.active_count + (int64_t)vmStats.inactive_count + (int64_t)vmStats.wire_count) *
        (int64_t)pageSize;

    qlonglong totalMemory = freeMemory + totalMemoryUsed;

    float percent = static_cast<float>(static_cast<double>(totalMemoryUsed) / static_cast<double>(totalMemory) * 100.0);
    return qBound(0.0f, percent, 100.0f);
}

float SystemInfoMac::cpuTemperature() {
    SMCOpen();
    double temperature = SMCGetTemperature(SMC_KEY_CPU_TEMP);
    SMCClose();
    return temperature;
}

float SystemInfoMac::cpuLoadAverage() {
    // credits: https://stackoverflow.com/a/20476923
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
    qCDebug(lcSysInfo) << "system:" << static_cast<double>(delta.totalSystemTime) / onePercent
                       << "user:" << static_cast<double>(delta.totalUserTime) / onePercent
                       << "idle:" << static_cast<double>(delta.totalIdleTime) / onePercent;

    auto percent = (delta.totalSystemTime + delta.totalUserTime) / onePercent;
    return qBound(0.0f, static_cast<float>(percent), 100.0f);
}

bool SystemInfoMac::getCpuSample(SystemInfoMac::CpuSample *sample) {
    // credits: https://stackoverflow.com/a/20476923
    processor_cpu_load_info_t cpuLoad;
    mach_msg_type_number_t    processorMsgCount;
    natural_t                 processorCount;

    uint64_t totalSystemTime = 0, totalUserTime = 0, totalIdleTime = 0;

    kern_return_t kr = host_processor_info(mach_host_self(), PROCESSOR_CPU_LOAD_INFO, &processorCount,
                                           reinterpret_cast<processor_info_array_t *>(&cpuLoad), &processorMsgCount);
    if (kr != KERN_SUCCESS) {
        qCCritical(lcSysInfo) << "Error getting host processor information" << mach_error_string(kr);
        return false;
    }

    for (natural_t i = 0; i < processorCount; i++) {
        uint64_t system = 0, user = 0, idle = 0;

        system = cpuLoad[i].cpu_ticks[CPU_STATE_SYSTEM];
        user = cpuLoad[i].cpu_ticks[CPU_STATE_USER] + cpuLoad[i].cpu_ticks[CPU_STATE_NICE];
        idle = cpuLoad[i].cpu_ticks[CPU_STATE_IDLE];

        totalSystemTime += system;
        totalUserTime += user;
        totalIdleTime += idle;
    }

    sample->totalSystemTime = totalSystemTime;
    sample->totalUserTime = totalUserTime;
    sample->totalIdleTime = totalIdleTime;

    return true;
}
