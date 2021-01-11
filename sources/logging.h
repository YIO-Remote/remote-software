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

#pragma once

#include <QtDebug>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(lcApp);
Q_DECLARE_LOGGING_CATEGORY(lcI18n);
Q_DECLARE_LOGGING_CATEGORY(lcApi);
Q_DECLARE_LOGGING_CATEGORY(lcPlugin);
Q_DECLARE_LOGGING_CATEGORY(lcLogger);
Q_DECLARE_LOGGING_CATEGORY(lcMediaPlayer);
Q_DECLARE_LOGGING_CATEGORY(lcNotification);

Q_DECLARE_LOGGING_CATEGORY(lcEntity);
Q_DECLARE_LOGGING_CATEGORY(lcEntityRemote);

Q_DECLARE_LOGGING_CATEGORY(lcOta);
Q_DECLARE_LOGGING_CATEGORY(lcOtaDownload);

Q_DECLARE_LOGGING_CATEGORY(lcCfg);
Q_DECLARE_LOGGING_CATEGORY(lcCfgJson);

Q_DECLARE_LOGGING_CATEGORY(lcHwFactory);
Q_DECLARE_LOGGING_CATEGORY(lcBtnHandler);
Q_DECLARE_LOGGING_CATEGORY(lcTouch);
Q_DECLARE_LOGGING_CATEGORY(lcStandby);

Q_DECLARE_LOGGING_CATEGORY(lcBluetooth);
Q_DECLARE_LOGGING_CATEGORY(lcWifi);
Q_DECLARE_LOGGING_CATEGORY(lcWifiMock);
Q_DECLARE_LOGGING_CATEGORY(lcWifiWpaCtrl);
Q_DECLARE_LOGGING_CATEGORY(lcWifiShellScript);

Q_DECLARE_LOGGING_CATEGORY(lcDevice);
Q_DECLARE_LOGGING_CATEGORY(lcApds9960);
Q_DECLARE_LOGGING_CATEGORY(lcApds9960Gesture);
Q_DECLARE_LOGGING_CATEGORY(lcApds9960Light);
Q_DECLARE_LOGGING_CATEGORY(lcApds9960Prox);
Q_DECLARE_LOGGING_CATEGORY(lcDevPortExp);
Q_DECLARE_LOGGING_CATEGORY(lcDevHaptic);
Q_DECLARE_LOGGING_CATEGORY(lcDevBatCharger);
Q_DECLARE_LOGGING_CATEGORY(lcDevFuelGauge);
Q_DECLARE_LOGGING_CATEGORY(lcDevDisplay);

Q_DECLARE_LOGGING_CATEGORY(lcEnv);
Q_DECLARE_LOGGING_CATEGORY(lcSysInfo);
Q_DECLARE_LOGGING_CATEGORY(lcService);
Q_DECLARE_LOGGING_CATEGORY(lcSystemd);
Q_DECLARE_LOGGING_CATEGORY(lcServiceMock);
Q_DECLARE_LOGGING_CATEGORY(lcServiceWeb);
