#include "bq27441.h"

BQ27441::BQ27441()
{
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

    setCapacity(2500);

    qDebug() << "Control status" << getControlStatus();
    qDebug() << "Full charge capacity:" << getFullChargeCapacity();
    qDebug() << "Nominal available capacity:" << getNominalAvailableCapacity();
    qDebug() << "Remaining capacity:" << getRemainingCapacity();
    qDebug() << "Voltage:" << getVoltage();
    qDebug() << "State of charge:" << getStateOfCharge();
    qDebug() << "Health:" << getStateOfHealth();
    qDebug() << "Temperature:" << getTemperatureC();
    qDebug() << "Average current:" << getAverageCurrent();
}

BQ27441::~BQ27441() {}

void BQ27441::setCapacity(uint16_t capacity)
{
    // unseal the device
    wiringPiI2CWriteReg8(bus, 0x00, 0x00);
    wiringPiI2CWriteReg8(bus, 0x01, 0x80);
    wiringPiI2CWriteReg8(bus, 0x00, 0x00);
    wiringPiI2CWriteReg8(bus, 0x01, 0x80);

    // SET_CFGUPDATE
    wiringPiI2CWriteReg8(bus, 0x00, 0x13);
    wiringPiI2CWriteReg8(bus, 0x01, 0x00);

    // check flag
    wiringPiI2CReadReg8(bus, 0x06);
    delay(1000);
    uint8_t checkBit = (uint8_t) wiringPiI2CReadReg8(bus, 0x06);
    if ((checkBit) & (1<<(4))) {
        // bit changed, let's configure

        // block data control command
        wiringPiI2CWriteReg8(bus, 0x61, 0x00);

        // data block class command
        wiringPiI2CWriteReg8(bus, 0x3e, 0x52);

        // write block offset location
        wiringPiI2CWriteReg8(bus, 0x3f, 0x00);

        // read old checksum
        uint8_t old_checksum = (uint8_t) wiringPiI2CReadReg8(bus, 0x60);

        // read existing capacity
        uint8_t old_descap_msb = (uint8_t) wiringPiI2CReadReg8(bus, 0x4a);
        uint8_t old_descap_lsb = (uint8_t) wiringPiI2CReadReg8(bus, 0x4b);

        // write new capacity
        uint8_t new_descap_msb = capacity >> 8;
        uint8_t new_descap_lsb = capacity & 0x00ff;

        wiringPiI2CWriteReg8(bus, 0x4a, new_descap_msb);
        wiringPiI2CWriteReg8(bus, 0x4b, new_descap_lsb);
        delay(1000);

        // calculate new checksum
        uint8_t temp = (255 - old_checksum - old_descap_lsb - old_descap_msb) % 256;
        uint8_t new_checksum = 255 - ((temp + new_descap_lsb + new_descap_msb) % 256);

        // write new checksum
        wiringPiI2CWriteReg8(bus, 0x60, new_checksum);
        delay(1000);

        // soft reset
        wiringPiI2CWriteReg8(bus, 0x00, 0x42);
        wiringPiI2CWriteReg8(bus, 0x01, 0x00);
        delay(500);

        // check flag
        wiringPiI2CReadReg8(bus, 0x06);
        delay(1000);
        checkBit = (uint8_t) wiringPiI2CReadReg8(bus, 0x06);
        if (!((checkBit) & (1<<(4)))) {
            // bit changed, let's seal
            wiringPiI2CWriteReg8(bus, 0x00, 0x20);
            wiringPiI2CWriteReg8(bus, 0x01, 0x00);
            delay(500);
        }
    }
}

int16_t BQ27441::getTemperatureC() {  // Result in 0.1 Celcius
    int16_t result;
    uint16_t raw;

    raw = (uint16_t) wiringPiI2CReadReg16(bus,BQ27441_COMMAND_TEMP);
    // Convert to 0.1 Celsius using integer math
    result = (int16_t) raw - 2731;
    return result;
}

uint16_t BQ27441::getVoltage() {
    uint16_t result;

    result = (uint16_t) wiringPiI2CReadReg16(bus,BQ27441_COMMAND_VOLTAGE);
    return result;
}

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

uint16_t BQ27441::getFullAvailableCapacity() {
    uint16_t result;

    result = (uint16_t) wiringPiI2CReadReg16(bus,BQ27441_COMMAND_AVAIL_CAPACITY);
    return result;
}

uint16_t BQ27441::getRemainingCapacity() {
    uint16_t result;

    result = (uint16_t) wiringPiI2CReadReg16(bus,BQ27441_COMMAND_REM_CAPACITY);
    return result;
}

uint16_t BQ27441::getFullChargeCapacity() {
    uint16_t result;

    result = (uint16_t) wiringPiI2CReadReg16(bus,BQ27441_COMMAND_FULL_CAPACITY);
    return result;
}

int16_t  BQ27441::getAverageCurrent() {
    uint16_t result;

    result = (uint16_t) wiringPiI2CReadReg16(bus,BQ27441_COMMAND_AVG_CURRENT);
    return (int16_t) result;
}

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

int16_t  BQ27441::getAveragePower() {
    uint16_t result;

    result = (uint16_t) wiringPiI2CReadReg16(bus,BQ27441_COMMAND_AVG_POWER);
    return (int16_t) result;
}

uint16_t BQ27441::getStateOfCharge() {
    uint16_t result;

    result = (uint16_t) wiringPiI2CReadReg16(bus,BQ27441_COMMAND_SOC);
    return result;
}

int16_t  BQ27441::getInternalTemperatureC() {  // Result in 0.1 Celsius
    int16_t result;
    uint16_t raw;

    raw = (uint16_t) wiringPiI2CReadReg16(bus,BQ27441_COMMAND_INT_TEMP);
    // Convert to 0.1 Celsius using integer math
    result = (int16_t) raw - 2731;
    return result;
}

uint16_t BQ27441::getStateOfHealth() {
    uint16_t result;

    result = (uint16_t) wiringPiI2CReadReg8(bus,BQ27441_COMMAND_SOH);
    return (result);
}

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
    return result;
}

uint16_t BQ27441::getDesignCapacity() {
    uint16_t result;

    result = (uint16_t) wiringPiI2CReadReg16(bus,BQ27441_EXTENDED_CAPACITY);
    return result;
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

uint16_t BQ27441::readRegister(uint8_t regAddress){
    uint16_t result;

    result = (uint16_t) wiringPiI2CReadReg16(bus,regAddress);
    return result;
}

uint16_t BQ27441::getControlWord(uint8_t subCommand) {
    uint16_t result;

    // Since the MSB of all the CONTROL subcommands is zero, an unsigned 8-bit
    // value is unchanged when casting to a 16-bit value.
    wiringPiI2CWriteReg16(bus,BQ27441_COMMAND_CONTROL,(uint16_t) subCommand);
    result = (uint16_t) wiringPiI2CReadReg16(bus,BQ27441_COMMAND_CONTROL);

    return result;
}






