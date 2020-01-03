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

#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include "../displaycontrol.h"

class DisplayControlYio : public DisplayControl {
    Q_OBJECT

    // define timing
    struct timespec ts = {0, 40L};
    struct timespec ts2 = {0, 100L};
    struct timespec ts3 = {0, 300L};

 public:
    Q_INVOKABLE bool setMode(Mode mode) override;
    Q_INVOKABLE void setBrightness(int from, int to) override;

    explicit DisplayControlYio(QObject *parent = nullptr);

 private:
    void spi_screenreg_set(int32_t Addr, int32_t Data0, int32_t Data1);
};
