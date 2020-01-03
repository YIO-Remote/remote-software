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

#include "device.h"

Device::~Device() {
    if (m_open) {
        close();
    }
}

bool Device::isOpen() const { return m_open; }

bool Device::open() {
    m_open = true;
    m_errorString.clear();

    return true;
}

void Device::close() {
    if (!m_open) {
        return;
    }

    emit aboutToClose();
    m_open = false;
}

QString Device::errorString() const { return m_errorString; }

void Device::setErrorString(const QString &str) { m_errorString = str; }
