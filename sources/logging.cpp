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

#include "logging.h"

// Main modules
Q_LOGGING_CATEGORY(lcApp, "yio.app");
Q_LOGGING_CATEGORY(lcI18n, "yio.app.i18n");
Q_LOGGING_CATEGORY(lcApi, "yio.api");
Q_LOGGING_CATEGORY(lcPlugin, "yio.plugin");
Q_LOGGING_CATEGORY(lcLogger, "yio.logger");
Q_LOGGING_CATEGORY(lcMediaPlayer, "yio.mediaplayer");
Q_LOGGING_CATEGORY(lcNotification, "yio.notification");

// Entities
Q_LOGGING_CATEGORY(lcEntity, "yio.entity");
Q_LOGGING_CATEGORY(lcEntityRemote, "yio.entity.remote");

// Updates
Q_LOGGING_CATEGORY(lcOta, "yio.ota");
Q_LOGGING_CATEGORY(lcOtaDownload, "yio.ota.download");

// Configuration
Q_LOGGING_CATEGORY(lcCfg, "yio.cfg");
Q_LOGGING_CATEGORY(lcCfgJson, "yio.cfg.json");

// Hardware
Q_LOGGING_CATEGORY(lcHwFactory, "yio.hw.factory");
Q_LOGGING_CATEGORY(lcBtnHandler, "yio.hw.btnhandler");
Q_LOGGING_CATEGORY(lcTouch, "yio.hw.touchevent");
Q_LOGGING_CATEGORY(lcStandby, "yio.hw.standby");

Q_LOGGING_CATEGORY(lcBluetooth, "yio.bluetooth");
Q_LOGGING_CATEGORY(lcWifi, "yio.wifi");
Q_LOGGING_CATEGORY(lcWifiMock, "yio.wifi.mock");
Q_LOGGING_CATEGORY(lcWifiWpaCtrl, "yio.wifi.wpactrl");
Q_LOGGING_CATEGORY(lcWifiShellScript, "yio.wifi.script");

// Hardware devices
Q_LOGGING_CATEGORY(lcDevice, "yio.dev.device");
Q_LOGGING_CATEGORY(lcApds9960, "yio.dev.apds9960");
Q_LOGGING_CATEGORY(lcApds9960Gesture, "yio.dev.apds9960.gesture");
Q_LOGGING_CATEGORY(lcApds9960Light, "yio.dev.apds9960.light");
Q_LOGGING_CATEGORY(lcApds9960Prox, "yio.dev.apds9960.prox");
Q_LOGGING_CATEGORY(lcDevPortExp, "yio.dev.portexp");
Q_LOGGING_CATEGORY(lcDevHaptic, "yio.dev.haptic");
Q_LOGGING_CATEGORY(lcDevBatCharger, "yio.dev.bat.charger");
Q_LOGGING_CATEGORY(lcDevFuelGauge, "yio.dev.bat.fuelgauge");
Q_LOGGING_CATEGORY(lcDevDisplay, "yio.dev.display");

// OS environment
Q_LOGGING_CATEGORY(lcEnv, "yio.env");
Q_LOGGING_CATEGORY(lcSysInfo, "yio.env.sysinfo");
Q_LOGGING_CATEGORY(lcService, "yio.env.srv");
Q_LOGGING_CATEGORY(lcSystemd, "yio.env.srv.systemd");
Q_LOGGING_CATEGORY(lcServiceMock, "yio.env.srv.mock");
Q_LOGGING_CATEGORY(lcServiceWeb, "yio.env.srv.web");
