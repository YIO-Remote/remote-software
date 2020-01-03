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

#include "bq27441.h"

#include <unistd.h>

#include <QLoggingCategory>
#include <QtDebug>
#include "QFile"

#include "../../notifications.h"

static Q_LOGGING_CATEGORY(CLASS_LC, "BQ27441");

BQ27441::BQ27441(const QString &i2cDevice, int i2cDeviceId, QObject *parent)
    : BatteryFuelGauge(parent), m_i2cDevice(i2cDevice), m_i2cDeviceId(i2cDeviceId), m_i2cFd(0) {}

BQ27441::~BQ27441() { close(); }

bool BQ27441::open() {
    /* Initialize I2C */
    m_i2cFd = wiringPiI2CSetupInterface(qPrintable(m_i2cDevice), m_i2cDeviceId);
    if (m_i2cFd == -1) {
        qCCritical(CLASS_LC) << "Unable to open or select I2C device" << m_i2cDeviceId << "on" << m_i2cDevice;
    } else {
        Device::open();
    }

    // Get device type and set lipo_status
    /// To be implemented later
    uint16_t bq27441_device_id = getDeviceType();

    if (bq27441_device_id != BQ27441_DEVICE_ID) {
        setErrorString(tr("Cannot initialize the battery sensor. Please restart the remote."));
        return false;
    } else {
        qCDebug(CLASS_LC) << "Successfully opened";
    }

    return isOpen();
}

void BQ27441::close() {
    Device::close();

    if (m_i2cFd > 0) {
        ::close(m_i2cFd);
        m_i2cFd = 0;
    }
}

void BQ27441::begin() {
    if (!isOpen()) {
        return;
    }
    /*        QFile poweronFile("/usr/bin/yio-remote/poweron");
    if (poweronFile.exists()) {
        // remove the file, so on a reboot we won't calibare the gauge again
        poweronFile.remove();

        // calibrate the gauge
        qDebug() << "Fuel gauge calibration. Setting charge capacity to:" << m_capacity;
        // changeCapacity(m_capacity);
    } else */
    if (getDesignCapacity() != m_capacity) {
        qDebug() << "Design capacity does not match.";

        // calibrate the gauge
        qDebug() << "Fuel gauge calibration. Setting charge capacity to:" << m_capacity;
        changeCapacity(m_capacity);
    }
}

void BQ27441::changeCapacity(int newCapacity) {
    if (!isOpen()) {
        return;
    }

    uint16_t capacity = uint16_t(newCapacity);
    // unseal the device
    wiringPiI2CWriteReg8(m_i2cFd, 0x00, 0x00);
    wiringPiI2CWriteReg8(m_i2cFd, 0x01, 0x80);
    wiringPiI2CWriteReg8(m_i2cFd, 0x00, 0x00);
    wiringPiI2CWriteReg8(m_i2cFd, 0x01, 0x80);
    delay(5);

    // SET_CFGUPDATE
    wiringPiI2CWriteReg8(m_i2cFd, 0x00, 0x13);
    wiringPiI2CWriteReg8(m_i2cFd, 0x01, 0x00);
    delay(1000);

    // check flag
    wiringPiI2CWrite(m_i2cFd, 0x06);
    delay(5);
    wiringPiI2CReadReg8(m_i2cFd, 0x06);
    delay(1000);
    uint8_t checkBit = (uint8_t)wiringPiI2CReadReg8(m_i2cFd, 0x06);
    if ((checkBit) & (1 << (4))) {
        // bit changed, let's configure

        // block data control command
        wiringPiI2CWriteReg8(m_i2cFd, 0x61, 0x00);
        delay(5);

        // data block class command
        wiringPiI2CWriteReg8(m_i2cFd, 0x3e, 0x52);
        delay(5);

        // write block offset location
        wiringPiI2CWriteReg8(m_i2cFd, 0x3f, 0x00);
        delay(5);

        // read old checksum
        wiringPiI2CWrite(m_i2cFd, 0x60);
        delay(5);
        uint8_t old_checksum = (uint8_t)wiringPiI2CReadReg8(m_i2cFd, 0x60);
        delay(5);

        // read existing capacity
        wiringPiI2CWrite(m_i2cFd, 0x4a);
        delay(5);
        uint8_t old_descap_msb = (uint8_t)wiringPiI2CReadReg8(m_i2cFd, 0x4a);
        uint8_t old_descap_lsb = (uint8_t)wiringPiI2CReadReg8(m_i2cFd, 0x4b);
        delay(5);

        // write new capacity
        uint8_t new_descap_msb = capacity >> 8;
        uint8_t new_descap_lsb = capacity & 0x00ff;

        wiringPiI2CWriteReg8(m_i2cFd, 0x4a, new_descap_msb);
        wiringPiI2CWriteReg8(m_i2cFd, 0x4b, new_descap_lsb);
        delay(1000);

        // read existing energy
        wiringPiI2CWrite(m_i2cFd, 0x4c);
        delay(5);
        uint8_t old_desen = (uint8_t)wiringPiI2CReadReg8(m_i2cFd, 0x4c);
        delay(5);

        // write new energy
        uint8_t new_desen = (uint8_t)(capacity * 3.8) >> 8;

        wiringPiI2CWriteReg8(m_i2cFd, 0x4c, new_desen);
        delay(5);

        // read terminate voltage
        wiringPiI2CWrite(m_i2cFd, 0x50);
        delay(5);
        uint8_t old_termv = (uint8_t)wiringPiI2CReadReg8(m_i2cFd, 0x50);

        // write terminate voltage
        uint8_t new_termv = (uint8_t)(3300) >> 8;

        wiringPiI2CWriteReg8(m_i2cFd, 0x50, new_termv);
        delay(5);

        // read taper rate
        wiringPiI2CWrite(m_i2cFd, 0x5b);
        delay(5);
        uint8_t old_taper = (uint8_t)wiringPiI2CReadReg8(m_i2cFd, 0x5b);

        // write taper rate
        uint8_t new_taper = (uint8_t)(217) >> 8;

        // calculate new checksum
        uint8_t temp = (255 - old_checksum - old_descap_lsb - old_descap_msb - old_desen - old_termv - old_taper) % 256;
        uint8_t new_checksum =
            255 - ((temp + new_descap_lsb + new_descap_msb + new_desen + new_termv + new_taper) % 256);

        // write new checksum
        wiringPiI2CWriteReg8(m_i2cFd, 0x60, new_checksum);
        delay(1000);

        // soft reset
        wiringPiI2CWriteReg8(m_i2cFd, 0x00, 0x42);
        wiringPiI2CWriteReg8(m_i2cFd, 0x01, 0x00);
        delay(500);

        // check flag
        wiringPiI2CWrite(m_i2cFd, 0x06);
        delay(500);
        wiringPiI2CReadReg8(m_i2cFd, 0x06);
        delay(500);
        checkBit = (uint8_t)wiringPiI2CReadReg8(m_i2cFd, 0x06);
        if (!((checkBit) & (1 << (4)))) {
            // bit changed, let's seal
            wiringPiI2CWriteReg8(m_i2cFd, 0x00, 0x20);
            wiringPiI2CWriteReg8(m_i2cFd, 0x01, 0x00);
            delay(500);
        }
    }
}

int32_t BQ27441::getTemperatureC() {  // Result in 1 Celcius
    if (!isOpen()) {
        return 0;
    }
    int32_t result;
    uint16_t raw;

    raw = (uint16_t)wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_TEMP);
    result = ((int32_t)raw / 10) - 273;
    return result;
}

int BQ27441::getVoltage() {
    if (!isOpen()) {
        return 0;
    }
    int result;
    result = wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_VOLTAGE);
    return result;
}

uint16_t BQ27441::getFlags() {
    if (!isOpen()) {
        return 0;
    }
    uint16_t result;

    result = (uint16_t)wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_FLAGS);
    return result;
}

uint16_t BQ27441::getNominalAvailableCapacity() {
    if (!isOpen()) {
        return 0;
    }
    uint16_t result;

    result = (uint16_t)wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_NOM_CAPACITY);
    return result;
}

int BQ27441::getFullAvailableCapacity() {
    if (!isOpen()) {
        return 0;
    }
    uint16_t result;

    result = (uint16_t)wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_AVAIL_CAPACITY);
    return static_cast<int>(result);
}

int BQ27441::getRemainingCapacity() {
    if (!isOpen()) {
        return 0;
    }
    uint16_t result;

    result = (uint16_t)wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_REM_CAPACITY);
    return static_cast<int>(result);
}

int BQ27441::getFullChargeCapacity() {
    if (!isOpen()) {
        return 0;
    }
    int result;

    result = (uint16_t)wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_FULL_CAPACITY);
    return static_cast<int>(result);
}

int BQ27441::getAverageCurrent() {
    if (!isOpen()) {
        return 0;
    }
    int16_t result;

    result = int16_t(wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_AVG_CURRENT));
    return static_cast<int>(result);
}

int16_t BQ27441::getStandbyCurrent() {
    if (!isOpen()) {
        return 0;
    }
    uint16_t result;

    result = (uint16_t)wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_STDBY_CURRENT);
    return (int16_t)result;
}

int16_t BQ27441::getMaxLoadCurrent() {
    if (!isOpen()) {
        return 0;
    }
    uint16_t result;

    result = (uint16_t)wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_MAX_CURRENT);
    return (int16_t)result;
}

int BQ27441::getAveragePower() {
    if (!isOpen()) {
        return 0;
    }
    int16_t result;

    result = (int16_t)wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_AVG_POWER);
    return static_cast<int>(result);
}

int BQ27441::getStateOfCharge() {
    if (!isOpen()) {
        return 0;
    }

    int result = wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_SOC);
    if (result < 0) {
        close();
        Notifications::getInstance()->add(true, tr("Battery sensor communication error. Please restart the remote."));
    }
    return result;
}

int16_t BQ27441::getInternalTemperatureC() {  // Result in 0.1 Celsius
    if (!isOpen()) {
        return 0;
    }
    int16_t result;
    uint16_t raw;

    raw = (uint16_t)wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_INT_TEMP);
    // Convert to 0.1 Celsius using integer math
    result = (int16_t)raw - 2731;
    return result;
}

int BQ27441::getStateOfHealth() {
    if (!isOpen()) {
        return 0;
    }
    uint8_t result;

    uint16_t raw = (uint16_t)wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_SOH);
    result = raw & 0x0ff;
    return static_cast<int>(result);
}

uint16_t BQ27441::getRemainingCapacityUnfiltered() {
    if (!isOpen()) {
        return 0;
    }
    uint16_t result;

    result = (uint16_t)wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_REM_CAP_UNFL);
    return result;
}

uint16_t BQ27441::getRemainingCapacityFiltered() {
    if (!isOpen()) {
        return 0;
    }
    uint16_t result;

    result = (uint16_t)wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_REM_CAP_FIL);
    return result;
}

uint16_t BQ27441::getFullChargeCapacityUnfiltered() {
    if (!isOpen()) {
        return 0;
    }
    uint16_t result;

    result = (uint16_t)wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_FULL_CAP_UNFL);
    return result;
}

uint16_t BQ27441::getFullChargeCapacityFiltered() {
    if (!isOpen()) {
        return 0;
    }
    uint16_t result;

    result = (uint16_t)wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_FULL_CAP_FIL);
    return result;
}

uint16_t BQ27441::getStateOfChargeUnfiltered() {
    if (!isOpen()) {
        return 0;
    }
    uint16_t result;

    result = (uint16_t)wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_SOC_UNFL);
    return result;
}

uint16_t BQ27441::getOpConfig() {
    if (!isOpen()) {
        return 0;
    }
    uint16_t result;

    result = (uint16_t)wiringPiI2CReadReg16(m_i2cFd, BQ27441_EXTENDED_OPCONFIG);
    if ((result & (1 << 5)) == 0) {
        qDebug("Device is in sleep mode");
    }

    return result;
}

int BQ27441::getDesignCapacity() {
    if (!isOpen()) {
        return 0;
    }
    uint16_t result;

    result = (uint16_t)wiringPiI2CReadReg16(m_i2cFd, BQ27441_EXTENDED_CAPACITY);
    return static_cast<int>(result);
}

uint16_t BQ27441::getControlStatus() {
    if (!isOpen()) {
        return 0;
    }
    uint16_t result;

    wiringPiI2CWriteReg16(m_i2cFd, BQ27441_COMMAND_CONTROL, (uint16_t)BQ27441_CONTROL_STATUS);
    result = (uint16_t)wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_CONTROL);
    return result;
}

uint16_t BQ27441::getDeviceType() {
    if (!isOpen()) {
        return 0;
    }
    uint16_t result;

    wiringPiI2CWriteReg16(m_i2cFd, BQ27441_COMMAND_CONTROL, (uint16_t)BQ27441_CONTROL_DEVICE_TYPE);
    result = (uint16_t)wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_CONTROL);

    return result;
}

uint16_t BQ27441::getChemID() {
    if (!isOpen()) {
        return 0;
    }
    uint16_t result;

    wiringPiI2CWriteReg16(m_i2cFd, BQ27441_COMMAND_CONTROL, (uint16_t)BQ27441_CONTROL_CHEM_ID);
    result = (uint16_t)wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_CONTROL);

    return result;
}

void BQ27441::reset() {
    if (!isOpen()) {
        return;
    }
    // unseal the device
    wiringPiI2CWriteReg8(m_i2cFd, 0x00, 0x00);
    wiringPiI2CWriteReg8(m_i2cFd, 0x01, 0x80);
    wiringPiI2CWriteReg8(m_i2cFd, 0x00, 0x00);
    wiringPiI2CWriteReg8(m_i2cFd, 0x01, 0x80);
    delay(5);

    // send reset command
    wiringPiI2CWriteReg8(m_i2cFd, 0x00, 0x41);
    wiringPiI2CWriteReg8(m_i2cFd, 0x01, 0x00);
    delay(200);
}
