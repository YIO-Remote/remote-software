/******************************************************************************
 *
 * Copyright (C) 2020 Foo Bar <foo@bar.com>
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

Environment::Environment(QObject *parent) : QObject(parent) {}

QString Environment::getDeviceType() {
    // Quick and dirty for now
    QString cpu =
#if defined(Q_PROCESSOR_X86_64)
        ":x86_64";
#elif defined(Q_PROCESSOR_X86_32)
        ":x86_32";
#elif defined(Q_PROCESSOR_ARM)
        ":arm";
#else
        "";
#endif

#if defined(Q_OS_ANDROID)
    return QString("android") + cpu;
#elif defined(Q_OS_IOS)
    return "ios";
#elif defined(Q_OS_LINUX)
#if defined(Q_PROCESSOR_ARM)
    // TODO(zehnm) further check if running on yio remote hardware or RPi!
    return "remote";
#else
    return QString("linux") + cpu;
#endif
#elif defined(Q_OS_WIN64)
    return "windows:64";
#elif defined(Q_OS_WIN32)
        return "windows:32";
#elif defined(Q_OS_MACOS)
        return "mac";
#else
        return "other";
#endif
}
