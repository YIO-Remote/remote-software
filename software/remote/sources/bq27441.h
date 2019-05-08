#ifndef BQ27441_H
#define BQ27441_H

#include <QObject>
#include <QtDebug>

#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/*****************************************************************************/
// BQ27441 Device definitions
//
// Default I2C Address
#define BQ27441_I2C_ADDRESS 0x55
// Default Device ID -- expect this value to be returned from readDeviceID()
#define BQ27441_DEVICE_ID	0x0421
// Registers (aka commands) that can be used with readRegister()
// See sections 4.2 - 4.20 and 5.1 - 5.2 in Technical Reference (SLUUAC9A)
#define BQ27441_COMMAND_TEMP			      0x02 // 0.1 Kelvins
#define BQ27441_COMMAND_VOLTAGE			    0x04 // mV
#define BQ27441_COMMAND_FLAGS			      0x06 // See section 4.4
#define BQ27441_COMMAND_NOM_CAPACITY	  0x08 // mAh
#define BQ27441_COMMAND_AVAIL_CAPACITY	0x0A // mAh
#define BQ27441_COMMAND_REM_CAPACITY	  0x0C // mAh
#define BQ27441_COMMAND_FULL_CAPACITY	  0x0E // mAh
#define BQ27441_COMMAND_AVG_CURRENT		  0x10 // mA
#define BQ27441_COMMAND_STDBY_CURRENT	  0x12 // mA
#define BQ27441_COMMAND_MAX_CURRENT		  0x14 // mA
#define BQ27441_COMMAND_AVG_POWER		    0x18 // mW
#define BQ27441_COMMAND_SOC				      0x1C // %
#define BQ27441_COMMAND_INT_TEMP		    0x1E // 0.1 Kelvins
#define BQ27441_COMMAND_SOH             0x20 // %
#define BQ27441_COMMAND_REM_CAP_UNFL	  0x28 // mAh
#define BQ27441_COMMAND_REM_CAP_FIL		  0x2A // mAh
#define BQ27441_COMMAND_FULL_CAP_UNFL	  0x2C // mAh
#define BQ27441_COMMAND_FULL_CAP_FIL	  0x2E // mAh
#define BQ27441_COMMAND_SOC_UNFL		    0x30 // %
#define BQ27441_EXTENDED_OPCONFIG	      0x3A // See section 5.1
#define BQ27441_EXTENDED_CAPACITY	      0x3C // See section 5.2
// Command opcode. Used internally by  getControlWord()
// See section 4.1 in Technical Reference (SLUUAC9A)
#define BQ27441_COMMAND_CONTROL			    0x00 //
// Subcommands used with getControlWord
// See sections 4.1.1 - 4.1.6 in Technical Reference (SLUUAC9A)
#define BQ27441_CONTROL_STATUS          0x00 // See section 4.1.1
#define BQ27441_CONTROL_DEVICE_TYPE     0x01 // Should always return 0x0421
#define BQ27441_CONTROL_FW_VERSION      0x02 // See section 4.1.3
#define BQ27441_CONTROL_DM_CODE         0x04 // See section 4.1.4
#define BQ27441_CONTROL_PREV_MACWRITE   0x07 // See section 4.1.5
#define BQ27441_CONTROL_CHEM_ID         0x08 // bq27441-G1A should return 0x0128 and bq27441-G1B should return 0x0312.
/*****************************************************************************/


class BQ27441 : public QObject
{

    Q_OBJECT

public:

    BQ27441();
    ~BQ27441();

    void begin();
    int16_t  getTemperatureC();                 // Result in 0.1 Celsius
    uint16_t getVoltage();
    uint16_t getFlags();
    uint16_t getNominalAvailableCapacity();
    uint16_t getFullAvailableCapacity();
    uint16_t getRemainingCapacity();
    uint16_t getFullChargeCapacity();
    int16_t  getAverageCurrent();
    int16_t  getStandbyCurrent();
    int16_t  getMaxLoadCurrent();
    int16_t  getAveragePower();
    uint16_t getStateOfCharge();
    uint16_t getInternalTemperature();            // Result in 0.1 Kelvins
    int16_t  getInternalTemperatureC();           // Result in 0.1 Celsius
    int16_t  getInternalTemperatureF();           // Result in 0.1 Fahrenheit
    uint16_t getStateOfHealth();
    uint16_t getRemainingCapacityUnfiltered();
    uint16_t getRemainingCapacityFiltered();
    uint16_t getFullChargeCapacityUnfiltered();
    uint16_t getFullChargeCapacityFiltered();
    uint16_t getStateOfChargeUnfiltered();
    // Extended Data Commands
    uint16_t getOpConfig();
    uint16_t getDesignCapacity();
    // Control Subcommands
    uint16_t getControlStatus();
    uint16_t getDeviceType();
//    uint16_t getFWVersion();
//    uint16_t getDMCode();
//    uint16_t getPrevMacwrite();
    uint16_t getChemID();

    void setCapacity(uint16_t capacity);

    uint16_t readRegister(uint8_t regAddress);
    uint16_t getControlWord(uint8_t subCommand);

private:
    int bus;
};

#endif // BQ27441_H
