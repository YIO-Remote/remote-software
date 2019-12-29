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

#ifndef INTERRUPT_HANDLER_H
#define INTERRUPT_HANDLER_H

#include <QObject>
#include <QFile>
#include <QSocketNotifier>
#include <QtDebug>

#include "../interrupthandler.h"

#include "mcp23017_handler.h"

class Mcp23017InterruptHandler : public InterruptHandler
{
    Q_OBJECT

public:

    Mcp23017InterruptHandler(QObject *parent = nullptr) : InterruptHandler(parent)
    {
        setupGPIO();
    }

    //~Mcp23017InterruptHandler() override {}

    Q_INVOKABLE virtual void shutdown() override
    {
        mcp.shutdown();
    }

    virtual QString getButton() override {
        return m_button;
    }

private:

    void setupGPIO()
    {
        QFile exportFile("/sys/class/gpio/export");
        if (!exportFile.open(QIODevice::WriteOnly)) {
            qDebug() << "Error opening: /sys/class/gpio/export";
            return;
        }
        exportFile.write("18");
        exportFile.close();

        QFile directionFile("/sys/class/gpio/gpio18/direction");
        if (!directionFile.open(QIODevice::WriteOnly)) {
            qDebug() << "Error opening: /sys/class/gpio/gpio18/direction";
            return;
        }
        directionFile.write("in");
        directionFile.close();

        QFile edgeFile("/sys/class/gpio/gpio18/edge");
        if (!edgeFile.open(QIODevice::WriteOnly)) {
            qDebug() << "Error opening: /sys/class/gpio/gpio18/edge";
            return;
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
    }

    void interruptHandler()
    {
        QFile file("/sys/class/gpio/gpio18/value");
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << "Error opening: /sys/class/gpio/gpio18/value";
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

private:
    QString         m_button;
    MCP23017        mcp = MCP23017();
    QSocketNotifier *notifier;
    QFile           *file;

};

#endif // INTERRUPT_HANDLER_H
