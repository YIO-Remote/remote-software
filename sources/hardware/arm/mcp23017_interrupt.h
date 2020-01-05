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

#pragma once

#include <QFile>
#include <QSocketNotifier>

#include "../interrupthandler.h"
#include "mcp23017_handler.h"

class Mcp23017InterruptHandler : public InterruptHandler {
    Q_OBJECT

 public:
    explicit Mcp23017InterruptHandler(const QString &i2cDevice = "/dev/i2c-3", int i2cDeviceId = MCP23017_ADDRESS,
                                      QObject *parent = nullptr);

    // ~Mcp23017InterruptHandler() override {}

    Q_INVOKABLE void shutdown() override { mcp.shutdown(); }

    // Device interface
 public:
    bool open() override;

 private:
    bool setupGPIO();

    void interruptHandler();

 private:
    QString          m_i2cDevice;
    int              m_i2cDeviceId;
    MCP23017         mcp = MCP23017();
    QSocketNotifier *notifier;
    QFile *          file;
};
