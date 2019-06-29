#include "QFile"
#include "bq27441.h"

BQ27441::BQ27441()
{
#ifdef __arm__
    /* Initialize I2C */
    bus = wiringPiI2CSetupInterface("/dev/i2c-3", BQ27441_I2C_ADDRESS);

    uint16_t bq27441_device_id;

    // Get device type and set lipo_status
    /// To be implemented later
    bq27441_device_id = getDeviceType();

    if (bq27441_device_id != BQ27441_DEVICE_ID)
    {
        qDebug() << "Error: Unable to communicate with BQ27441.";
    }
#endif
}

BQ27441::~BQ27441() {}

void BQ27441::begin()
{
#ifdef __arm__
    QFile poweronFile("/usr/bin/yio-remote/poweron");
    if (poweronFile.exists()) {
        // remove the file, so on a reboot we won't calibare the gauge again
        poweronFile.remove();

        // calibrate the gauge
        qDebug() << "Fuel gauge calibration. Setting charge capacity to:" << m_capacity;
        changeCapacity((uint16_t(m_capacity)));
    }
#endif
}

#ifdef __arm__
void BQ27441::changeCapacity(uint16_t capacity)
{
    // unseal the device
    wiringPiI2CWriteReg8(bus, 0x00, 0x00);
    wiringPiI2CWriteReg8(bus, 0x01, 0x80);
    wiringPiI2CWriteReg8(bus, 0x00, 0x00);
    wiringPiI2CWriteReg8(bus, 0x01, 0x80);
    delay(5);

    // SET_CFGUPDATE
    wiringPiI2CWriteReg8(bus, 0x00, 0x13);
    wiringPiI2CWriteReg8(bus, 0x01, 0x00);
    delay(1000);

    // check flag
    wiringPiI2CWrite(bus, 0x06);
    delay(5);
    wiringPiI2CReadReg8(bus, 0x06);
    delay(1000);
    uint8_t checkBit = (uint8_t) wiringPiI2CReadReg8(bus, 0x06);
    if ((checkBit) & (1<<(4))) {
        // bit changed, let's configure

        // block data control command
        wiringPiI2CWriteReg8(bus, 0x61, 0x00);
        delay(5);

        // data block class command
        wiringPiI2CWriteReg8(bus, 0x3e, 0x52);
        delay(5);

        // write block offset location
        wiringPiI2CWriteReg8(bus, 0x3f, 0x00);
        delay(5);

        // read old checksum
        wiringPiI2CWrite(bus, 0x60);
        delay(5);
        uint8_t old_checksum = (uint8_t) wiringPiI2CReadReg8(bus, 0x60);
        delay(5);

        // read existing capacity
        wiringPiI2CWrite(bus, 0x4a);
        delay(5);
        uint8_t old_descap_msb = (uint8_t) wiringPiI2CReadReg8(bus, 0x4a);
        uint8_t old_descap_lsb = (uint8_t) wiringPiI2CReadReg8(bus, 0x4b);
        delay(5);

        // write new capacity
        uint8_t new_descap_msb = capacity >> 8;
        uint8_t new_descap_lsb = capacity & 0x00ff;

        wiringPiI2CWriteReg8(bus, 0x4a, new_descap_msb);
        wiringPiI2CWriteReg8(bus, 0x4b, new_descap_lsb);
        delay(1000);

        // read existing energy
        wiringPiI2CWrite(bus, 0x4c);
        delay(5);
        uint8_t old_desen = (uint8_t) wiringPiI2CReadReg8(bus, 0x4c);
        delay(5);

        // write new energy
        uint8_t new_desen = (uint8_t) (capacity*3.8) >> 8;

        wiringPiI2CWriteReg8(bus, 0x4c, new_desen);
        delay(5);

        // read terminate voltage
        wiringPiI2CWrite(bus, 0x50);
        delay(5);
        uint8_t old_termv = (uint8_t) wiringPiI2CReadReg8(bus, 0x50);

        // write terminate voltage
        uint8_t new_termv = (uint8_t) (3300) >> 8;

        wiringPiI2CWriteReg8(bus, 0x50, new_termv);
        delay(5);

        // read taper rate
        wiringPiI2CWrite(bus, 0x5b);
        delay(5);
        uint8_t old_taper = (uint8_t) wiringPiI2CReadReg8(bus, 0x5b);

        // write taper rate
        uint8_t new_taper = (uint8_t) (217) >> 8;

        // calculate new checksum
        uint8_t temp = (255 - old_checksum - old_descap_lsb - old_descap_msb - old_desen - old_termv - old_taper) % 256;
        uint8_t new_checksum = 255 - ((temp + new_descap_lsb + new_descap_msb + new_desen + new_termv + new_taper) % 256);

        // write new checksum
        wiringPiI2CWriteReg8(bus, 0x60, new_checksum);
        delay(1000);

        // soft reset
        wiringPiI2CWriteReg8(bus, 0x00, 0x42);
        wiringPiI2CWriteReg8(bus, 0x01, 0x00);
        delay(500);

        // check flag
        wiringPiI2CWrite(bus, 0x06);
        delay(500);
        wiringPiI2CReadReg8(bus, 0x06);
        delay(500);
        checkBit = (uint8_t) wiringPiI2CReadReg8(bus, 0x06);
        if (!((checkBit) & (1<<(4)))) {
            // bit changed, let's seal
            wiringPiI2CWriteReg8(bus, 0x00, 0x20);
            wiringPiI2CWriteReg8(bus, 0x01, 0x00);
            delay(500);
        }
    }
}

int32_t BQ27441::getTemperatureC() {  // Result in 1 Celcius
    int32_t result;
    uint16_t raw;

    raw = (uint16_t) wiringPiI2CReadReg16(bus,BQ27441_COMMAND_TEMP);
    result = ((int32_t) raw / 10 ) - 273;
    return result;
}
#endif

int BQ27441::getVoltage() {
#ifdef __arm__
    int result;
    result = wiringPiI2CReadReg16(bus,BQ27441_COMMAND_VOLTAGE);
    return result;
#else
    return 5000;
#endif
}

#ifdef __arm__
uint16_t BQ27441::getFlags() {
    uint16_t result;

    result = (uint16_t) wiringPiI2CReadReg16(bus,BQ27441_COMMAND_FLAGS);
    return result;
}

uint16_t BQ27441::getNominalAvailableCapacity() {
    uint16_t result;

    result = (uint16_t) wiringPiI2CReadReg16(bus,BQ27441_COMMAND_NOM_CAPACITY);
    return result;
}

int BQ27441::getFullAvailableCapacity() {
    uint16_t result;

    result = (uint16_t) wiringPiI2CReadReg16(bus,BQ27441_COMMAND_AVAIL_CAPACITY);
    return int(result);
}

uint16_t BQ27441::getRemainingCapacity() {
    uint16_t result;

    result = (uint16_t) wiringPiI2CReadReg16(bus,BQ27441_COMMAND_REM_CAPACITY);
    return result;
}
#endif

int BQ27441::getFullChargeCapacity() {
#ifdef __arm__
    int result;

    result = (uint16_t) wiringPiI2CReadReg16(bus,BQ27441_COMMAND_FULL_CAPACITY);
    return int(result);
#else
    return 2500;
#endif
}

int16_t  BQ27441::getAverageCurrent() {
#ifdef __arm__
    uint16_t result;

    result = (uint16_t) wiringPiI2CReadReg16(bus,BQ27441_COMMAND_AVG_CURRENT);
    return (int16_t) result;
#else
    return -1600;
#endif
}

#ifdef __arm__
int16_t  BQ27441::getStandbyCurrent() {
    uint16_t result;

    result = (uint16_t) wiringPiI2CReadReg16(bus,BQ27441_COMMAND_STDBY_CURRENT);
    return (int16_t) result;
}

int16_t  BQ27441::getMaxLoadCurrent() {
    uint16_t result;

    result = (uint16_t) wiringPiI2CReadReg16(bus,BQ27441_COMMAND_MAX_CURRENT);
    return (int16_t) result;
}
#endif

int BQ27441::getAveragePower() {
#ifdef __arm__
    int16_t result;

    result = (int16_t) wiringPiI2CReadReg16(bus,BQ27441_COMMAND_AVG_POWER);
    return (int) result;
#else
    return -2500;
#endif
}

int BQ27441::getStateOfCharge() {
#ifdef __arm__
    int result;

    result = wiringPiI2CReadReg16(bus,BQ27441_COMMAND_SOC);
    return result;
#else
    return 100;
#endif
}

int16_t  BQ27441::getInternalTemperatureC() {  // Result in 0.1 Celsius
#ifdef __arm__
    int16_t result;
    uint16_t raw;

    raw = (uint16_t) wiringPiI2CReadReg16(bus,BQ27441_COMMAND_INT_TEMP);
    // Convert to 0.1 Celsius using integer math
    result = (int16_t) raw - 2731;
    return result;
#else
    return 23;
#endif
}

int BQ27441::getStateOfHealth() {
#ifdef __arm__
    uint8_t result;

    uint16_t raw = (uint16_t) wiringPiI2CReadReg16(bus,BQ27441_COMMAND_SOH);
    result = raw & 0x0ff;
    return (int) result;
#else
    return 100;
#endif
}

#ifdef __arm__
uint16_t BQ27441::getRemainingCapacityUnfiltered() {
    uint16_t result;

    result = (uint16_t) wiringPiI2CReadReg16(bus,BQ27441_COMMAND_REM_CAP_UNFL);
    return result;
}

uint16_t BQ27441::getRemainingCapacityFiltered() {
    uint16_t result;

    result = (uint16_t) wiringPiI2CReadReg16(bus,BQ27441_COMMAND_REM_CAP_FIL);
    return result;
}

uint16_t BQ27441::getFullChargeCapacityUnfiltered() {
    uint16_t result;

    result = (uint16_t) wiringPiI2CReadReg16(bus,BQ27441_COMMAND_FULL_CAP_UNFL);
    return result;
}

uint16_t BQ27441::getFullChargeCapacityFiltered() {
    uint16_t result;

    result = (uint16_t) wiringPiI2CReadReg16(bus,BQ27441_COMMAND_FULL_CAP_FIL);
    return result;
}

uint16_t BQ27441::getStateOfChargeUnfiltered() {
    uint16_t result;

    result = (uint16_t) wiringPiI2CReadReg16(bus,BQ27441_COMMAND_SOC_UNFL);
    return result;
}

uint16_t BQ27441::getOpConfig() {
    uint16_t result;

    result = (uint16_t) wiringPiI2CReadReg16(bus,BQ27441_EXTENDED_OPCONFIG);
    if ((result & (1 << 5)) == 0) {
        qDebug("Device is in sleep mode");
    }

    return result;
}

int BQ27441::getDesignCapacity() {
    uint16_t result;

    result = (uint16_t) wiringPiI2CReadReg16(bus,BQ27441_EXTENDED_CAPACITY);
    return int(result);
}

uint16_t BQ27441::getControlStatus() {
    uint16_t result;

    wiringPiI2CWriteReg16(bus,BQ27441_COMMAND_CONTROL,(uint16_t) BQ27441_CONTROL_STATUS);
    result = (uint16_t) wiringPiI2CReadReg16(bus,BQ27441_COMMAND_CONTROL);
    return result;
}

uint16_t BQ27441::getDeviceType() {
    uint16_t result;

    wiringPiI2CWriteReg16(bus,BQ27441_COMMAND_CONTROL,(uint16_t) BQ27441_CONTROL_DEVICE_TYPE);
    result = (uint16_t) wiringPiI2CReadReg16(bus,BQ27441_COMMAND_CONTROL);

    return result;
}

uint16_t BQ27441::getChemID() {
    uint16_t result;

    wiringPiI2CWriteReg16(bus,BQ27441_COMMAND_CONTROL,(uint16_t) BQ27441_CONTROL_CHEM_ID);
    result = (uint16_t) wiringPiI2CReadReg16(bus,BQ27441_COMMAND_CONTROL);

    return result;
}

void BQ27441::reset()
{
    // unseal the device
    wiringPiI2CWriteReg8(bus, 0x00, 0x00);
    wiringPiI2CWriteReg8(bus, 0x01, 0x80);
    wiringPiI2CWriteReg8(bus, 0x00, 0x00);
    wiringPiI2CWriteReg8(bus, 0x01, 0x80);
    delay(5);

    // send reset command
    wiringPiI2CWriteReg8(bus, 0x00, 0x41);
    wiringPiI2CWriteReg8(bus, 0x01, 0x00);
    delay(200);
}
#endif




