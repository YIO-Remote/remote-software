/******************************************************************************
 *
 * Copyright (C) 2020 Markus Zehnder <business@markuszehnder.ch>
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

static Q_LOGGING_CATEGORY(CLASS_LC, "hw.dev.MCP23017");

Mcp23017InterruptHandler::Mcp23017InterruptHandler(const QString& i2cDevice, int i2cDeviceId, int gpio, QObject* parent)
    : InterruptHandler("Mcp23017InterruptHandler", parent)
    , m_i2cDevice(i2cDevice), m_i2cDeviceId(i2cDeviceId), m_gpio(gpio) {
    m_gpioValueDevice = QString("/sys/class/gpio/gpio%1/value").arg(gpio);
}

bool Mcp23017InterruptHandler::open() {
    if (isOpen()) {
        qCWarning(CLASS_LC) << DBG_WARN_DEVICE_OPEN;
        return true;
    }

    if (!mcp.setup(m_i2cDevice, m_i2cDeviceId)) {
        setErrorString(ERR_DEV_INTR_INIT);
        emit error(DeviceError::InitializationError, ERR_DEV_INTR_INIT);
        return false;
    }

    if (!setupGPIO()) {
        setErrorString(ERR_DEV_INTR_INIT);
        emit error(DeviceError::InitializationError, ERR_DEV_INTR_INIT);
        return false;
    }

    return Device::open();
}

bool Mcp23017InterruptHandler::setupGPIO() {
    qCDebug(CLASS_LC) << "Using GPIO device" << m_gpioValueDevice;

    QFile exportFile("/sys/class/gpio/export");
    if (!exportFile.open(QIODevice::WriteOnly)) {
        qCCritical(CLASS_LC) << "Error opening: /sys/class/gpio/export";
        return false;
    }

    QByteArray interruptVal = QString::number(m_gpio).toLocal8Bit();
    qCDebug(CLASS_LC) << "Configuring interrupt on GPIO" << interruptVal;
    exportFile.write(interruptVal);
    exportFile.close();

    QString directionDev = QString("/sys/class/gpio/gpio%1/direction").arg(m_gpio);
    QFile directionFile(directionDev);
    if (!directionFile.open(QIODevice::WriteOnly)) {
        qCCritical(CLASS_LC) << "Error opening:" << directionDev;
        return false;
    }
    directionFile.write("in");
    directionFile.close();

    QString edgeDev = QString("/sys/class/gpio/gpio%1/edge").arg(m_gpio);
    QFile edgeFile(edgeDev);
    if (!edgeFile.open(QIODevice::WriteOnly)) {
        qCCritical(CLASS_LC) << "Error opening:" << edgeDev;
        return false;
    }
    edgeFile.write("falling");
    edgeFile.close();

    // GPIO to look at; This is connected to the MCP23017 INTA&INTB ports
    file = new QFile(m_gpioValueDevice);
    file->open(QIODevice::ReadOnly);

    // connect to a signal
    notifier = new QSocketNotifier(file->handle(), QSocketNotifier::Exception);
    notifier->setEnabled(true);
    connect(notifier, &QSocketNotifier::activated, this, &Mcp23017InterruptHandler::interruptHandler);

    return true;
}

void Mcp23017InterruptHandler::interruptHandler() {
    QFile file(m_gpioValueDevice);
    if (!file.open(QIODevice::ReadOnly)) {
        qCCritical(CLASS_LC) << "Error opening:" << m_gpioValueDevice;
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

const QLoggingCategory &Mcp23017InterruptHandler::logCategory() const {
    return CLASS_LC();
}
