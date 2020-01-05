/******************************************************************************
 *
 * Copyright (C) 2020 Markus Zehnder <business@markuszehnder.ch>
 * Copyright (C) 2018-2019 Marton Borzak <hello@martonborzak.com>
 *
 * Third party work used:
 *
 *  Copyright (c) 2017, Adafruit Industries
 *  Software License Agreement (BSD License)
 *  All rights reserved.
 *
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

#include <unistd.h>

#include "../device.h"
#include "../proximitysensor.h"
#include "apds9960.h"

static Q_LOGGING_CATEGORY(CLASS_LC, "hw.dev.APDS9960");

APDS9960::APDS9960(const QString &i2cDevice, int i2cDeviceId, QObject *parent)
    : Device("APDS9960 sensor", parent), m_i2cDevice(i2cDevice), m_i2cDeviceId(i2cDeviceId), m_i2cFd(0) {}

APDS9960::~APDS9960() { close(); }

bool APDS9960::open() {
    if (isOpen()) {
        qCWarning(CLASS_LC) << DBG_WARN_DEVICE_OPEN;
        return true;
    }

    bool initialized = false;
    m_i2cFd = wiringPiI2CSetupInterface(qPrintable(m_i2cDevice), m_i2cDeviceId);
    if (m_i2cFd == -1) {
        qCCritical(CLASS_LC) << "Unable to open or select I2C device" << m_i2cDeviceId << "on" << m_i2cDevice;
    } else {
        uint8_t x = uint8_t(wiringPiI2CReadReg8(m_i2cFd, APDS9960_ID));
        if (x == 0xAB) {
            Device::open();
            initialized = begin();
        }
    }

    if (!initialized) {
        setErrorString(ERR_DEV_PROXIMITY_INIT);
        emit error(InitializationError, ERR_DEV_PROXIMITY_INIT);
        return false;
    }

    return true;
}

void APDS9960::close() {
    Device::close();

    if (m_i2cFd > 0) {
        ::close(m_i2cFd);
        m_i2cFd = 0;
    }
}

const QLoggingCategory &APDS9960::logCategory() const { return CLASS_LC(); }

void APDS9960::enable(bool en) {
    ASSERT_DEVICE_OPEN()

    _enable.PON = en;
    wiringPiI2CWriteReg8(m_i2cFd, APDS9960_ENABLE, _enable.get());
}

bool APDS9960::check() {
    // Check if the sensor is still there on the I2C bus
    uint8_t x = uint8_t(wiringPiI2CReadReg8(m_i2cFd, APDS9960_ID));
    if (x != 0xAB) {
        emit error(CommunicationError, ERR_DEV_PROXIMITY_COMM);
        return false;
    }
    return true;
}

bool APDS9960::begin(uint16_t iTimeMS, apds9960AGain_t aGain) {
    /* Set default integration time and gain */
    setADCIntegrationTime(iTimeMS);
    setADCGain(aGain);

    wiringPiI2CWriteReg8(m_i2cFd, APDS9960_ATIME, 219);
    wiringPiI2CWriteReg8(m_i2cFd, APDS9960_WTIME, 246);

    // disable everything to start
    enableGesture(false);
    enableProximity(false);
    enableColor(false);

    // proximity pulse
    wiringPiI2CWriteReg8(m_i2cFd, APDS9960_PPULSE, 0x87);

    // proximity offset
    wiringPiI2CWriteReg8(m_i2cFd, APDS9960_POFFSET_UR, 0);
    wiringPiI2CWriteReg8(m_i2cFd, APDS9960_POFFSET_DL, 0);

    disableColorInterrupt();
    disableProximityInterrupt();
    clearInterrupt();

    wiringPiI2CWriteReg8(m_i2cFd, APDS9960_CONFIG1, _config1.get());
    wiringPiI2CWriteReg8(m_i2cFd, APDS9960_CONFIG2, _config2.get());
    wiringPiI2CWriteReg8(m_i2cFd, APDS9960_CONFIG3, _config3.get());

    /* Note: by default, the device is in power down mode on bootup */
    enable(false);
    delay(10);
    enable(true);
    delay(10);

    // default to all gesture dimensions
    setGestureDimensions(APDS9960_DIMENSIONS_ALL);
    setGestureFIFOThreshold(APDS9960_GFIFO_4);
    setGestureGain(APDS9960_GGAIN_4);
    setGestureProximityThreshold(50);
    resetCounts();

    _gpulse.GPLEN = APDS9960_GPULSE_32US;
    _gpulse.GPULSE = 9;  // 10 pulses
    wiringPiI2CWriteReg8(m_i2cFd, APDS9960_GPULSE, _gpulse.get());

    return true;
}

void APDS9960::setADCIntegrationTime(uint16_t iTimeMS) {
    ASSERT_DEVICE_OPEN()

    float temp;

    // convert ms into 2.78ms increments
    temp = iTimeMS;
    temp /= 2.78f;
    temp = 256 - temp;
    if (temp > 255) {
        temp = 255;
    }
    if (temp < 0) {
        temp = 0;
    }

    /* Update the timing register */
    wiringPiI2CWriteReg8(m_i2cFd, APDS9960_ATIME, static_cast<uint8_t>(temp));
}

float APDS9960::getADCIntegrationTime() {
    ASSERT_DEVICE_OPEN(0)

    float temp;

    temp = static_cast<float>(wiringPiI2CReadReg8(m_i2cFd, APDS9960_ATIME));

    // convert to units of 2.78 ms
    temp = 256 - temp;
    temp *= 2.78f;
    return temp;
}

void APDS9960::setADCGain(apds9960AGain_t aGain) {
    ASSERT_DEVICE_OPEN()

    _control.AGAIN = aGain;

    /* Update the timing register */
    wiringPiI2CWriteReg8(m_i2cFd, APDS9960_CONTROL, _control.get());
}

apds9960AGain_t APDS9960::getADCGain() {
    ASSERT_DEVICE_OPEN(APDS9960_AGAIN_1X)

    return apds9960AGain_t((wiringPiI2CReadReg8(m_i2cFd, APDS9960_CONTROL) & 0x03));
}

void APDS9960::setProxGain(apds9960PGain_t pGain) {
    ASSERT_DEVICE_OPEN()

    _control.PGAIN = pGain;

    /* Update the timing register */
    wiringPiI2CWriteReg8(m_i2cFd, APDS9960_CONTROL, _control.get());
}

apds9960PGain_t APDS9960::getProxGain() {
    ASSERT_DEVICE_OPEN(APDS9960_PGAIN_1X)

    return apds9960PGain_t((wiringPiI2CReadReg8(m_i2cFd, APDS9960_CONTROL) & 0x0C));
}

void APDS9960::setProxPulse(apds9960PPulseLen_t pLen, uint8_t pulses) {
    ASSERT_DEVICE_OPEN()

    if (pulses < 1) {
        pulses = 1;
    }
    if (pulses > 64) {
        pulses = 64;
    }
    pulses--;

    _ppulse.PPLEN = pLen;
    _ppulse.PPULSE = pulses;

    wiringPiI2CWriteReg8(m_i2cFd, APDS9960_PPULSE, _ppulse.get());
}

void APDS9960::enableProximity(bool en) {
    ASSERT_DEVICE_OPEN()

    _enable.PEN = en;

    wiringPiI2CWriteReg8(m_i2cFd, APDS9960_ENABLE, _enable.get());
}

void APDS9960::enableProximityInterrupt() {
    ASSERT_DEVICE_OPEN()

    _enable.PIEN = 1;
    wiringPiI2CWriteReg8(m_i2cFd, APDS9960_ENABLE, _enable.get());
    clearInterrupt();
}

void APDS9960::disableProximityInterrupt() {
    ASSERT_DEVICE_OPEN()

    _enable.PIEN = 0;
    wiringPiI2CWriteReg8(m_i2cFd, APDS9960_ENABLE, _enable.get());
}

void APDS9960::setProximityInterruptThreshold(uint8_t low, uint8_t high, uint8_t persistance) {
    ASSERT_DEVICE_OPEN()

    wiringPiI2CWriteReg8(m_i2cFd, APDS9960_PILT, low);
    wiringPiI2CWriteReg8(m_i2cFd, APDS9960_PIHT, high);

    if (persistance > 7) {
        persistance = 7;
    }
    _pers.PPERS = persistance;
    wiringPiI2CWriteReg8(m_i2cFd, APDS9960_PERS, _pers.get());
}

bool APDS9960::getProximityInterrupt() {
    ASSERT_DEVICE_OPEN(false)

    _status.set(uint8_t(wiringPiI2CReadReg8(m_i2cFd, APDS9960_STATUS)));
    return _status.PINT;
}

uint8_t APDS9960::readProximity() {
    ASSERT_DEVICE_OPEN(0)

    return uint8_t(wiringPiI2CReadReg8(m_i2cFd, APDS9960_PDATA));
}

bool APDS9960::gestureValid() {
    if (!isOpen()) {
        qCWarning(CLASS_LC) << DBG_WARN_DEVICE_CLOSED;
        return false;
    }

    _gstatus.set(uint8_t(wiringPiI2CReadReg8(m_i2cFd, APDS9960_GSTATUS)));
    return _gstatus.GVALID;
}

void APDS9960::setGestureDimensions(uint8_t dims) {
    ASSERT_DEVICE_OPEN()

    _gconf3.GDIMS = dims;
    wiringPiI2CWriteReg8(m_i2cFd, APDS9960_GCONF3, _gconf3.get());
}

void APDS9960::setGestureFIFOThreshold(uint8_t thresh) {
    ASSERT_DEVICE_OPEN()

    _gconf1.GFIFOTH = thresh;
    wiringPiI2CWriteReg8(m_i2cFd, APDS9960_GCONF1, _gconf1.get());
}

void APDS9960::setGestureGain(uint8_t gain) {
    ASSERT_DEVICE_OPEN()

    _gconf2.GGAIN = gain;
    wiringPiI2CWriteReg8(m_i2cFd, APDS9960_GCONF2, _gconf2.get());
}

void APDS9960::setGestureProximityThreshold(uint8_t thresh) {
    ASSERT_DEVICE_OPEN()

    wiringPiI2CWriteReg8(m_i2cFd, APDS9960_GPENTH, thresh);
}

void APDS9960::setGestureOffset(uint8_t offset_up, uint8_t offset_down, uint8_t offset_left, uint8_t offset_right) {
    ASSERT_DEVICE_OPEN()

    wiringPiI2CWriteReg8(m_i2cFd, APDS9960_GOFFSET_U, offset_up);
    wiringPiI2CWriteReg8(m_i2cFd, APDS9960_GOFFSET_D, offset_down);
    wiringPiI2CWriteReg8(m_i2cFd, APDS9960_GOFFSET_L, offset_left);
    wiringPiI2CWriteReg8(m_i2cFd, APDS9960_GOFFSET_R, offset_right);
}

void APDS9960::enableGesture(bool en) {
    ASSERT_DEVICE_OPEN()

    if (!en) {
        _gconf4.GMODE = 0;
        wiringPiI2CWriteReg8(m_i2cFd, APDS9960_GCONF4, _gconf4.get());
    }
    _enable.GEN = en;
    wiringPiI2CWriteReg8(m_i2cFd, APDS9960_ENABLE, _enable.get());
    resetCounts();
}

void APDS9960::resetCounts() {
    gestCnt = 0;
    UCount = 0;
    DCount = 0;
    LCount = 0;
    RCount = 0;
}

void APDS9960::setLED(apds9960LedDrive_t drive, apds9960LedBoost_t boost) {
    ASSERT_DEVICE_OPEN()

    // set BOOST
    _config2.LED_BOOST = boost;
    wiringPiI2CWriteReg8(m_i2cFd, APDS9960_CONFIG2, _config2.get());

    _control.LDRIVE = drive;
    wiringPiI2CWriteReg8(m_i2cFd, APDS9960_CONTROL, _control.get());
}

void APDS9960::enableColor(bool en) {
    ASSERT_DEVICE_OPEN()

    _enable.AEN = en;
    wiringPiI2CWriteReg8(m_i2cFd, APDS9960_ENABLE, _enable.get());
}

bool APDS9960::colorDataReady() {
    ASSERT_DEVICE_OPEN(false)

    _status.set(uint8_t(wiringPiI2CReadReg8(m_i2cFd, APDS9960_STATUS)));
    return _status.AVALID;
}

void APDS9960::getColorData(uint16_t *r, uint16_t *g, uint16_t *b, uint16_t *c) {
    ASSERT_DEVICE_OPEN()

    *c = uint16_t(wiringPiI2CReadReg16(m_i2cFd, APDS9960_CDATAL));
    *r = uint16_t(wiringPiI2CReadReg16(m_i2cFd, APDS9960_RDATAL));
    *g = uint16_t(wiringPiI2CReadReg16(m_i2cFd, APDS9960_GDATAL));
    *b = uint16_t(wiringPiI2CReadReg16(m_i2cFd, APDS9960_BDATAL));
}

uint16_t APDS9960::getAmbientLight() {
    ASSERT_DEVICE_OPEN(0)

    uint16_t low = static_cast<uint16_t>(wiringPiI2CReadReg8(m_i2cFd, APDS9960_CDATAL));
    uint16_t high = static_cast<uint16_t>(wiringPiI2CReadReg8(m_i2cFd, APDS9960_CDATAH));

    return low + static_cast<uint16_t>(high << 8);
}

uint16_t APDS9960::calculateLux(uint16_t r, uint16_t g, uint16_t b) {
    ASSERT_DEVICE_OPEN(0)

    float illuminance;

    /* This only uses RGB ... how can we integrate clear or calculate lux */
    /* based exclusively on clear since this might be more reliable?      */
    illuminance = (-0.32466F * r) + (1.57837F * g) + (-0.73191F * b);

    return static_cast<uint16_t>(illuminance);
}

void APDS9960::enableColorInterrupt() {
    ASSERT_DEVICE_OPEN()

    _enable.AIEN = 1;
    wiringPiI2CWriteReg8(m_i2cFd, APDS9960_ENABLE, _enable.get());
}

void APDS9960::disableColorInterrupt() {
    ASSERT_DEVICE_OPEN()

    _enable.AIEN = 0;
    wiringPiI2CWriteReg8(m_i2cFd, APDS9960_ENABLE, _enable.get());
}

void APDS9960::clearInterrupt() {
    ASSERT_DEVICE_OPEN()

    //  write(APDS9960_AICLEAR, NULL, 0);
    wiringPiI2CWriteReg8(m_i2cFd, APDS9960_AICLEAR, 0x00);
    wiringPiI2CWriteReg8(m_i2cFd, APDS9960_PICLEAR, 0x00);
    wiringPiI2CWriteReg8(m_i2cFd, APDS9960_CICLEAR, 0x00);
}

void APDS9960::setIntLimits(uint16_t low, uint16_t high) {
    ASSERT_DEVICE_OPEN()

    wiringPiI2CWriteReg8(m_i2cFd, APDS9960_AILTIL, low & 0xFF);
    wiringPiI2CWriteReg8(m_i2cFd, APDS9960_AILTH, low >> 8);
    wiringPiI2CWriteReg8(m_i2cFd, APDS9960_AIHTL, high & 0xFF);
    wiringPiI2CWriteReg8(m_i2cFd, APDS9960_AIHTH, high >> 8);
}
