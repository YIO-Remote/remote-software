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

#include "bq27441.h"

#include <unistd.h>

#include <QLoggingCategory>
#include <QtDebug>

#include "../../../notifications.h"
#include "QFile"

static Q_LOGGING_CATEGORY(CLASS_LC, "hw.dev.BQ27441");

BQ27441::BQ27441(InterruptHandler *interruptHandler, const QString &i2cDevice, int i2cDeviceId, QObject *parent)
    : BatteryFuelGauge("BQ27441 battery fuel gauge", parent),
      m_i2cDevice(i2cDevice),
      m_i2cDeviceId(i2cDeviceId),
      m_i2cFd(0) {
    Q_ASSERT(interruptHandler);
    Q_ASSERT(!i2cDevice.isEmpty());
    qCDebug(CLASS_LC()) << name() << i2cDevice << "with id:" << i2cDeviceId;

    connect(interruptHandler, &InterruptHandler::interruptEvent, this, [&](int event) {
        if (event == InterruptHandler::BATTERY) {
            updateBatteryValues();
        }
    });
}

BQ27441::~BQ27441() { close(); }

bool BQ27441::open() {
    if (isOpen()) {
        qCWarning(CLASS_LC) << DBG_WARN_DEVICE_OPEN;
        return true;
    }

    /* Initialize I2C */
    bool initialized = false;
    m_i2cFd          = wiringPiI2CSetupInterface(qPrintable(m_i2cDevice), m_i2cDeviceId);
    if (m_i2cFd == -1) {
        qCCritical(CLASS_LC) << "Unable to open or select I2C device" << m_i2cDeviceId << "on" << m_i2cDevice;
    } else {
        // Get device type and set lipo_status
        /// To be implemented later
        uint16_t bq27441_device_id = getDeviceType();

        initialized = bq27441_device_id == BQ27441_DEVICE_ID;
    }

    if (!initialized) {
        qCCritical(CLASS_LC) << "Cannot initialize the BQ27441 battery sensor.";
        setErrorString(ERR_DEV_BATTERY_INIT);
        emit error(InitializationError, ERR_DEV_BATTERY_INIT);
        return false;
    }
    return Device::open();
}

void BQ27441::close() {
    Device::close();

    if (m_i2cFd > 0) {
        ::close(m_i2cFd);
        m_i2cFd = 0;
    }
}

const QLoggingCategory &BQ27441::logCategory() const { return CLASS_LC(); }

void BQ27441::updateBatteryValues() {
    if (getDesignCapacity() != m_capacity) {
        qCDebug(CLASS_LC) << "Design capacity does not match.";

        // calibrate the gauge
        qCDebug(CLASS_LC) << "Fuel gauge calibration. Setting charge capacity to:" << m_capacity;
        changeCapacity(m_capacity);
    }

    m_level = getStateOfCharge();
    emit levelChanged();
    qCDebug(CLASS_LC()) << "Battery level:" << m_level;

    m_voltage = wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_VOLTAGE);  // getVoltage();
    qCDebug(CLASS_LC()) << "Battery voltage:" << m_voltage;

    m_health = getStateOfHealth();
    emit healthChanged();
    qCDebug(CLASS_LC()) << "Battery health:" << m_health;

    m_averagePower = static_cast<int>(
        static_cast<int16_t>(wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_AVG_POWER)));  // getAveragePower();
    emit averagePowerChanged();

    if (m_level != -1) {
        // check for critical low power
        if (0 < m_voltage && m_voltage <= 3400 && m_averagePower < 0) {
            emit criticalLowBattery();
        }

        // check for low battery, below 20%
        if (m_level <= 20 && !m_wasLowBatteryWarning) {
            m_wasLowBatteryWarning = true;
            emit lowBattery();
        }

        // if it is charged a bit, reset the warning
        if (m_level > 20) {
            m_wasLowBatteryWarning = false;
        }

        // check if the battery is charging
        if (m_averagePower >= 0) {
            m_isCharging = true;
            qCDebug(CLASS_LC()) << "Battery is charging";
            emit isChargingChanged();
        } else {
            m_isCharging = false;
            qCDebug(CLASS_LC()) << "Battery is not charging";
            emit isChargingChanged();
        }

        // check if charging is finished
        if (m_averagePower == 0 && m_level == 1) {
            emit chargingDone();
        }
    }

    // calculate remaining battery life
    float remainingLife = static_cast<float>(getRemainingCapacity()) / static_cast<float>(abs(getAverageCurrent()));
    m_remainingLife     = (m_remainingLife + remainingLife) / 2;
    emit remainingLifeChanged();

    qCDebug(CLASS_LC()) << "Average power" << m_averagePower << "mW";
    qCDebug(CLASS_LC()) << "Average current" << getAverageCurrent() << "mA";
    qCDebug(CLASS_LC()) << "Remaining battery life" << m_remainingLife << "h";
}

void BQ27441::begin() {
    ASSERT_DEVICE_OPEN()
    updateBatteryValues();
    /*        QFile poweronFile("/usr/bin/yio-remote/poweron");
    if (poweronFile.exists()) {
        // remove the file, so on a reboot we won't calibare the gauge again
        poweronFile.remove();

        // calibrate the gauge
        qCDebug(CLASS_LC) << "Fuel gauge calibration. Setting charge capacity to:" << m_capacity;
        // changeCapacity(m_capacity);
    } else */
    //    if (getDesignCapacity() != m_capacity) {
    //        qCDebug(CLASS_LC) << "Design capacity does not match.";

    //        // calibrate the gauge
    //        qCDebug(CLASS_LC) << "Fuel gauge calibration. Setting charge capacity to:" << m_capacity;
    //        changeCapacity(m_capacity);
    //    }
}

void BQ27441::changeCapacity(int newCapacity) {
    ASSERT_DEVICE_OPEN()

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
    uint8_t checkBit = static_cast<uint8_t>(wiringPiI2CReadReg8(m_i2cFd, 0x06));
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
        uint8_t old_checksum = static_cast<uint8_t>(wiringPiI2CReadReg8(m_i2cFd, 0x60));
        delay(5);

        // read existing capacity
        wiringPiI2CWrite(m_i2cFd, 0x4a);
        delay(5);
        uint8_t old_descap_msb = static_cast<uint8_t>(wiringPiI2CReadReg8(m_i2cFd, 0x4a));
        uint8_t old_descap_lsb = static_cast<uint8_t>(wiringPiI2CReadReg8(m_i2cFd, 0x4b));
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
        uint8_t old_desen = static_cast<uint8_t>(wiringPiI2CReadReg8(m_i2cFd, 0x4c));
        delay(5);

        // write new energy
        uint8_t new_desen = static_cast<uint8_t>(capacity * 3.8) >> 8;

        wiringPiI2CWriteReg8(m_i2cFd, 0x4c, new_desen);
        delay(5);

        // read terminate voltage
        wiringPiI2CWrite(m_i2cFd, 0x50);
        delay(5);
        uint8_t old_termv = static_cast<uint8_t>(wiringPiI2CReadReg8(m_i2cFd, 0x50));

        // write terminate voltage
        uint8_t new_termv = static_cast<uint8_t>(3300) >> 8;

        wiringPiI2CWriteReg8(m_i2cFd, 0x50, new_termv);
        delay(5);

        // read taper rate
        wiringPiI2CWrite(m_i2cFd, 0x5b);
        delay(5);
        uint8_t old_taper = static_cast<uint8_t>(wiringPiI2CReadReg8(m_i2cFd, 0x5b));

        // write taper rate
        uint8_t new_taper = static_cast<uint8_t>(217) >> 8;

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
        checkBit = static_cast<uint8_t>(wiringPiI2CReadReg8(m_i2cFd, 0x06));
        if (!((checkBit) & (1 << (4)))) {
            // bit changed, let's seal
            wiringPiI2CWriteReg8(m_i2cFd, 0x00, 0x20);
            wiringPiI2CWriteReg8(m_i2cFd, 0x01, 0x00);
            delay(500);
        }
    }
}

int BQ27441::getLevel() { return m_level; }

int BQ27441::getHealth() { return m_health; }

bool BQ27441::getIsCharging() { return m_isCharging; }

float BQ27441::remainingLife() { return m_remainingLife; }

int BQ27441::getTemperatureC() {  // Result in 1 Celcius
    ASSERT_DEVICE_OPEN(0)

    int raw = wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_TEMP);
    return (raw / 10) - 273;
}

int BQ27441::getVoltage() {
    //    ASSERT_DEVICE_OPEN(0)

    //    return wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_VOLTAGE);
    return m_voltage;
}

uint16_t BQ27441::getFlags() {
    ASSERT_DEVICE_OPEN(0)

    return static_cast<uint16_t>(wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_FLAGS));
}

uint16_t BQ27441::getNominalAvailableCapacity() {
    ASSERT_DEVICE_OPEN(0)

    return static_cast<uint16_t>(wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_NOM_CAPACITY));
}

int BQ27441::getFullAvailableCapacity() {
    ASSERT_DEVICE_OPEN(0)

    return wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_AVAIL_CAPACITY);
}

int BQ27441::getRemainingCapacity() {
    ASSERT_DEVICE_OPEN(0)

    return wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_REM_CAPACITY);
}

int BQ27441::getFullChargeCapacity() {
    ASSERT_DEVICE_OPEN(0)

    return wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_FULL_CAPACITY);
}

int BQ27441::getAverageCurrent() {
    ASSERT_DEVICE_OPEN(0)

    return static_cast<int>(static_cast<int16_t>(wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_AVG_CURRENT)));
}

int16_t BQ27441::getStandbyCurrent() {
    ASSERT_DEVICE_OPEN(0)

    return static_cast<int16_t>(wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_STDBY_CURRENT));
}

int16_t BQ27441::getMaxLoadCurrent() {
    ASSERT_DEVICE_OPEN(0)

    return static_cast<int16_t>(wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_MAX_CURRENT));
}

int BQ27441::getAveragePower() {
    //    ASSERT_DEVICE_OPEN(0)

    // this is needed otherwise the values are weird
    //    return static_cast<int>(static_cast<int16_t>(wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_AVG_POWER)));
    return m_averagePower;
}

int BQ27441::getStateOfCharge() {
    ASSERT_DEVICE_OPEN(0)

    int result = wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_SOC);
    if (result < 0) {
        close();
        setErrorString(ERR_DEV_BATTERY_COMM);
        emit error(CommunicationError, ERR_DEV_BATTERY_COMM);
    }
    return result;
}

int16_t BQ27441::getInternalTemperatureC() {  // Result in 0.1 Celsius
    ASSERT_DEVICE_OPEN(0)

    int raw = wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_INT_TEMP);
    // Convert to 0.1 Celsius using integer math
    return static_cast<int16_t>(raw - 2731);
}

int BQ27441::getStateOfHealth() {
    ASSERT_DEVICE_OPEN(0)

    int raw = wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_SOH);
    return raw & 0x0ff;
}

uint16_t BQ27441::getRemainingCapacityUnfiltered() {
    ASSERT_DEVICE_OPEN(0)

    return static_cast<uint16_t>(wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_REM_CAP_UNFL));
}

uint16_t BQ27441::getRemainingCapacityFiltered() {
    ASSERT_DEVICE_OPEN(0)

    return static_cast<uint16_t>(wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_REM_CAP_FIL));
}

uint16_t BQ27441::getFullChargeCapacityUnfiltered() {
    ASSERT_DEVICE_OPEN(0)

    return static_cast<uint16_t>(wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_FULL_CAP_UNFL));
}

uint16_t BQ27441::getFullChargeCapacityFiltered() {
    ASSERT_DEVICE_OPEN(0)

    return static_cast<uint16_t>(wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_FULL_CAP_FIL));
}

uint16_t BQ27441::getStateOfChargeUnfiltered() {
    ASSERT_DEVICE_OPEN(0)

    return static_cast<uint16_t>(wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_SOC_UNFL));
}

uint16_t BQ27441::getOpConfig() {
    ASSERT_DEVICE_OPEN(0)

    uint16_t result = static_cast<uint16_t>(wiringPiI2CReadReg16(m_i2cFd, BQ27441_EXTENDED_OPCONFIG));
    if ((result & (1 << 5)) == 0) {
        qCDebug(CLASS_LC()) << "Device is in sleep mode";
    }

    return result;
}

int BQ27441::getDesignCapacity() {
    ASSERT_DEVICE_OPEN(0)

    return wiringPiI2CReadReg16(m_i2cFd, BQ27441_EXTENDED_CAPACITY);
}

uint16_t BQ27441::getControlStatus() {
    ASSERT_DEVICE_OPEN(0)

    wiringPiI2CWriteReg16(m_i2cFd, BQ27441_COMMAND_CONTROL, static_cast<uint16_t>(BQ27441_CONTROL_STATUS));
    return static_cast<uint16_t>(wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_CONTROL));
}

uint16_t BQ27441::getDeviceType() {
    // do NOT asssert open status: Called from within open() where the device is being initialized!
    wiringPiI2CWriteReg16(m_i2cFd, BQ27441_COMMAND_CONTROL, static_cast<uint16_t>(BQ27441_CONTROL_DEVICE_TYPE));
    return static_cast<uint16_t>(wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_CONTROL));
}

uint16_t BQ27441::getChemID() {
    ASSERT_DEVICE_OPEN(0)

    wiringPiI2CWriteReg16(m_i2cFd, BQ27441_COMMAND_CONTROL, static_cast<uint16_t>(BQ27441_CONTROL_CHEM_ID));
    return static_cast<uint16_t>(wiringPiI2CReadReg16(m_i2cFd, BQ27441_COMMAND_CONTROL));
}

void BQ27441::reset() {
    ASSERT_DEVICE_OPEN()

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
