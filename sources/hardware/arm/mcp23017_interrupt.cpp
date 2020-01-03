/******************************************************************************
 *
 * Copyright (C) 2018-2019 Marton Borzak <hello@martonborzak.com>
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

#include <QLoggingCategory>
#include <QtDebug>

#include "mcp23017_interrupt.h"

static Q_LOGGING_CATEGORY(CLASS_LC, "Mcp23017");

Mcp23017InterruptHandler::Mcp23017InterruptHandler(const QString& i2cDevice, int i2cDeviceId, QObject* parent)
    : InterruptHandler(parent), m_i2cDevice(i2cDevice), m_i2cDeviceId(i2cDeviceId) {}

bool Mcp23017InterruptHandler::open() {
    if (!mcp.setup(m_i2cDevice, m_i2cDeviceId)) {
        return false;
    }

    if (!setupGPIO()) {
        return false;
    }

    qCDebug(CLASS_LC) << "Successfully opened";
    return Device::open();
}

bool Mcp23017InterruptHandler::setupGPIO() {
    QFile exportFile("/sys/class/gpio/export");
    if (!exportFile.open(QIODevice::WriteOnly)) {
        qCCritical(CLASS_LC) << "Error opening: /sys/class/gpio/export";
        return false;
    }
    exportFile.write("18");
    exportFile.close();

    QFile directionFile("/sys/class/gpio/gpio18/direction");
    if (!directionFile.open(QIODevice::WriteOnly)) {
        qCCritical(CLASS_LC) << "Error opening: /sys/class/gpio/gpio18/direction";
        return false;
    }
    directionFile.write("in");
    directionFile.close();

    QFile edgeFile("/sys/class/gpio/gpio18/edge");
    if (!edgeFile.open(QIODevice::WriteOnly)) {
        qCCritical(CLASS_LC) << "Error opening: /sys/class/gpio/gpio18/edge";
        return false;
    }
    edgeFile.write("falling");
    edgeFile.close();

    // GPIO to look at; This is connected to the MCP23017 INTA&INTB ports
    file = new QFile("/sys/class/gpio/gpio18/value");
    file->open(QIODevice::ReadOnly);

    // connect to a signal
    notifier = new QSocketNotifier(file->handle(), QSocketNotifier::Exception);
    notifier->setEnabled(true);
    connect(notifier, &QSocketNotifier::activated, this, &Mcp23017InterruptHandler::interruptHandler);

    return true;
}

void Mcp23017InterruptHandler::interruptHandler() {
    QFile file("/sys/class/gpio/gpio18/value");
    if (!file.open(QIODevice::ReadOnly)) {
        qCCritical(CLASS_LC) << "Error opening: /sys/class/gpio/gpio18/value";
        return;
    }

    int gpioVal = file.readAll().toInt();

    // if the GPIO is 0, then it's a button press
    if (gpioVal == 0) {
        // check the MCP23017 what caused the interrupt
        m_button = mcp.readInterrupt();

        // tell qml that there was a button press
        emit buttonPressed();
    }

    delay(10);
}
