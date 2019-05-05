#ifndef BQ27441_H
#define BQ27441_H

#include <QObject>
#include <QtDebug>

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <math.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <time.h>
#include <sys/time.h>
#include <curses.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdint.h>

#define BQ27441_ADDR 0x55
#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

#define BQ27441_CONTROL_STATUS		0x0000
#define BQ27441_DEVICE_TYPE		0x0001
#define BQ27441_FW_VERSION		0x0002
#define BQ27441_DM_CODE			0x0004
#define BQ27441_PREV_MACWRITE		0x0007
#define BQ27441_CHEM_ID			0x0008
#define BQ27441_BAT_INSERT		0x000C
#define BQ27441_BAT_REMOVE		0x000D
#define BQ27441_SET_HIBERNATE		0x0011
#define BQ27441_CLEAR_HIBERNATE		0x0012
#define BQ27441_SET_CFGUPDATE		0x0013
#define BQ27441_SHUTDOWN_ENABLE		0x001B
#define BQ27441_SHUTDOWN		0x001C
#define BQ27441_SEALED			0x0020
#define BQ27441_PULSE_SOC_INT		0x0023
#define BQ27441_RESET			0x0041
#define BQ27441_SOFT_RESET		0x0042

#define BQ27441_CONTROL_1		0x00
#define BQ27441_CONTROL_2		0x01
#define BQ27441_TEMPERATURE		0x02
#define BQ27441_VOLTAGE			0x04
#define BQ27441_FLAGS			0x06
#define BQ27441_NOMINAL_AVAIL_CAPACITY	0x08
#define BQ27441_FULL_AVAIL_CAPACITY	0x0a
#define BQ27441_REMAINING_CAPACITY	0x0c
#define BQ27441_FULL_CHG_CAPACITY	0x0e
#define BQ27441_AVG_CURRENT		0x10
#define BQ27441_STANDBY_CURRENT		0x12
#define BQ27441_MAXLOAD_CURRENT		0x14
#define BQ27441_AVERAGE_POWER		0x18
#define BQ27441_STATE_OF_CHARGE		0x1c
#define BQ27441_INT_TEMPERATURE		0x1e
#define BQ27441_STATE_OF_HEALTH		0x20

#define BQ27441_BLOCK_DATA_CHECKSUM	0x60
#define BQ27441_BLOCK_DATA_CONTROL	0x61
#define BQ27441_DATA_BLOCK_CLASS	0x3E
#define BQ27441_DATA_BLOCK		0x3F

#define BQ27441_DESIGN_CAPACITY_1	0x4A
#define BQ27441_DESIGN_CAPACITY_2	0x4B
#define BQ27441_DESIGN_ENERGY   	0x4C
#define BQ27441_TERMINATE_VOLTAGE   	0x50

#define BQ27441_BATTERY_LOW		15
#define BQ27441_BATTERY_FULL		100

#define BQ27441_MAX_REGS		0x7F

typedef unsigned char byte;
int deviceDescriptor;

class BQ27441 {
public:
    /* This function initializes the I2C device*/

    BQ27441();
    ~BQ27441();

    void begin()
    {
        qDebug() << "**************************************** HELLLO ";

        int i, voltage;
        byte data[100], writeData[100];
        float remaining_batt_cap = 0.0;
        float full_charge_cap = 0.0;
        float soc = 0.0;
        float temp = 0.0;
        float current = 0.0;

        writeData[0] = 0x00;
        writeData[1] = 0x04;

        qDebug() << "Inside begin";

        init_i2c();

        config_gauge();

        /* Reading the device registers */
        I2cSendData(BQ27441_ADDR, writeData, 2);
        I2cReadData(BQ27441_ADDR, data, 100);

        voltage = data[4]*16*16 + data[3];
        remaining_batt_cap = data[12]*16*16 + data[11];
        full_charge_cap = data[14]*16*16 + data[13];
        soc = data[28]*16*16 + data[27];//(remaining_batt_cap/full_charge_cap)*100;
        temp = (data[2]*16*16 + data[1])/10.0 - 273.0;
        current = data[16]*16*16 + data[15];

        if (current >= 32267)
            current = current - 65536;          // two's complement as signed integer

        qDebug() << "Voltage: " << voltage;
        printf("Current: %f  mA\n", current);
        printf("Remaining Battery Capacity: %f  mAh\n", remaining_batt_cap);
        printf("Full Charge Capacity: %f mAh\n", full_charge_cap);
        printf("State of Charge: %f p.c. \n", soc);
        printf("Temperature: %f  Deg C\n", temp);
    }

    bool init_i2c()
    {
        qDebug() << "Initialising i2c device";
        deviceDescriptor = wiringPiI2CSetupInterface("/dev/i2c-3", BQ27441_ADDR);
        if(deviceDescriptor == -1) {
            return false;
        }
        return true;
    }

    bool I2cSendData(  uint8_t reg,
                       uint8_t *val,
                       unsigned int len)
    {
        if(wiringPiI2CWrite(deviceDescriptor, reg) < 0) {
            return false;
        }

        unsigned char i = 0;
        for(i = 0; i < len; i++) {
            if(wiringPiI2CWrite(deviceDescriptor, val[i]) < 0) {
                return false;
            }
        }
        return true;
    }

    int I2cReadData(   uint8_t reg,
                       uint8_t *val,
                       unsigned int len)
    {
        if(wiringPiI2CWrite(deviceDescriptor, reg) < 0) {
            return -1;
        }

        unsigned char i = 0;
        for(i = 0; i < len; i++) {
            uint8_t dat;
            if((dat = wiringPiI2CRead(deviceDescriptor)) < 0) {
                return -1;
            }
            val[i] = dat;
        }
        return i;
    }

    /* Convert the number to hexadecimal representation */
    void to_hex_16(char *output, unsigned n)
    {
        static const char hex_digits[] = "0123456789abcdef";
        output[0] = hex_digits[(n >> 12) & 0xF];
        output[1] = hex_digits[(n >> 8) & 0xF];
        output[2] = hex_digits[(n >> 4) & 0xF];
        output[3] = hex_digits[(n & 0xF)];
        output[4] = '\0';
    }

    static int checksum(byte *check_data)
    {
        int sum = 0;
        int ii = 0;

        for(ii = 0; ii < 32; ii++)
            sum += check_data[ii+62];

        sum &= 0xFF;

        return 0xFF - sum;
    }

    void config_gauge() {
        byte unseal_data[10], cfgupdate_data[10], flag_data[10], flag_out[10];
        byte block_data_control[10], data_block_class[10], data_block[10], block_data_checksum[10];
        byte block_data_checksum_data[10], design_capacity_loc[10], design_capacity_data[10];
        byte design_energy_data[10], terminate_voltage_data[10], des_cap[10], term_vol[10];
        byte design_energy_loc[10], terminate_voltage_loc[10], init_reg[100], init_data[100];
        byte soft_reset[10], seal_data[10];
        int i, design_capacity, new_design_capacity, new_design_cap_hex, design_energy;
        int cksum = 0, terminate_voltage,new_terminate_voltage;
        char new_design_cap[7], a[10], b[10], tmp[10], new_term_vol[7];

        init_reg[0] = 0x00;
        init_reg[1] = 0x04;

        unseal_data[0] = BQ27441_CONTROL_1;
        unseal_data[1] = 0x00;
        unseal_data[2] = 0x80;

        cfgupdate_data[0] = BQ27441_CONTROL_1;
        cfgupdate_data[1] = 0x13;
        cfgupdate_data[2] = 0x00;

        flag_data[0] = BQ27441_FLAGS;

        block_data_control[0] = BQ27441_BLOCK_DATA_CONTROL;
        block_data_control[1] = 0x00;

        data_block_class[0] = BQ27441_DATA_BLOCK_CLASS;
        data_block_class[1] = 0x52;

        data_block[0] = BQ27441_DATA_BLOCK;
        data_block[1] = 0x00;

        block_data_checksum[0] = BQ27441_BLOCK_DATA_CHECKSUM;

        design_capacity_loc[0] = BQ27441_DESIGN_CAPACITY_1;
        design_energy_loc[0] = BQ27441_DESIGN_ENERGY;
        terminate_voltage_loc[0] = BQ27441_TERMINATE_VOLTAGE;

        soft_reset[0] = 0x00;
        soft_reset[1] = 0x42;
        soft_reset[2] = 0x00;

        seal_data[0] = 0x00;
        seal_data[1] = 0x20;
        seal_data[2] = 0x00;

        /* A cursory read of all registers*/
        I2cSendData(BQ27441_ADDR, init_reg, 2);
        I2cReadData(BQ27441_ADDR, init_data, 100);

        /* Unseal the gauge - Refer TRM - Pg-14 */
        I2cSendData(BQ27441_ADDR, unseal_data, 3);      // #1
        I2cSendData(BQ27441_ADDR, unseal_data, 3);
        delay(5);
        printf("The gauge seems to be unsealed. \n");

        I2cSendData(BQ27441_ADDR, cfgupdate_data, 3);   // #2
        delay(1000);
        I2cSendData(BQ27441_ADDR, flag_data, 1);        // #3
        delay(5);
        I2cReadData(BQ27441_ADDR, flag_out, 1);

        printf("The flag_out is: %x \n", flag_out[0]);

        if (CHECK_BIT(flag_out[0], 4)) {
            printf("The gauge is ready to be configured \n");

            I2cSendData(BQ27441_ADDR, block_data_control, 2);       // #4
            delay(5);
            I2cSendData(BQ27441_ADDR, data_block_class, 2);         // #5
            delay(5);
            I2cSendData(BQ27441_ADDR, data_block, 2);               // #6
            delay(5);
            I2cSendData(BQ27441_ADDR, block_data_checksum, 1);      // #7
            delay(5);
            I2cReadData(BQ27441_ADDR, block_data_checksum_data, 1);
            delay(5);

            printf("The checksum_data: %x \n", block_data_checksum_data[0]);


            // printf("The checksum is as expected. Config will proceed. \n");

            /* Design Capacity */
            I2cSendData(BQ27441_ADDR, design_capacity_loc, 1);      // #8
            delay(5);
            I2cReadData(BQ27441_ADDR, design_capacity_data, 2);
            delay(5);

            //printf("Design capacity data: %x and %x \n", design_capacity_data[0], design_capacity_data[1]);

            design_capacity = design_capacity_data[0]*16*16 + design_capacity_data[1];
            delay(5);

            printf("The current design capacity is: %d mAh \n", design_capacity);
            //                        new_design_cap = "2500";

            printf("Trying to update the design capacity \n");

            design_capacity = 2500;
            if (design_capacity < 0)
                design_capacity = 0;
            else if (design_capacity > 8000)
                design_capacity = 8000;             // #9
            printf("Trying to set new design capacity to: %d \n", design_capacity);
            to_hex_16(tmp, design_capacity);

            for(i = 0; i <= 3; i++) {
                printf("Output at position %d has %c \n", i, tmp[i]);
            }

            des_cap[0] = BQ27441_DESIGN_CAPACITY_1;//design_capacity_loc[0];
            des_cap[1] = (tmp[0] - '0')*16 + (tmp[1] - '0');
            des_cap[2] = (tmp[2] - '0')*16 + (tmp[3] - '0');

            printf("Des cap 0: %d ", des_cap[0]);
            printf("Des cap 1: %d ", des_cap[1]);
            printf("Des cap 2: %d ", des_cap[2]);
            I2cSendData(BQ27441_ADDR, des_cap, 3);
            delay(1000);

            /* Design Energy */
            I2cSendData(BQ27441_ADDR, design_energy_loc, 1);      // #8
            delay(50);
            I2cReadData(BQ27441_ADDR, design_energy_data, 2);
            delay(50);

            design_energy = design_energy_data[0]*16*16 + design_energy_data[1];

            printf("The current design energy is: %d mWh \n", design_energy);
            printf("Setting the design energy as 3.8 times the design capacity. \n");
            design_energy = design_capacity * 3.8;  // standard G1B conversion formula
            delay(100);
            printf("The new design energy is: %d mWh \n ", design_energy);

            to_hex_16(tmp, new_design_capacity);

            design_energy_data[0] = BQ27441_DESIGN_ENERGY;
            design_energy_data[1] = (tmp[0] - '0')*16 + (tmp[1] - '0');
            design_energy_data[2] = (tmp[2] - '0')*16 + (tmp[3] - '0');

            I2cSendData(BQ27441_ADDR, design_energy_data, 3);
            delay(5);

            /* Terminate Voltage */
            I2cSendData(BQ27441_ADDR, terminate_voltage_loc, 1);      // #8
            delay(5);
            I2cReadData(BQ27441_ADDR, terminate_voltage_data, 2);
            delay(5);

            terminate_voltage = terminate_voltage_data[0]*16*16 + terminate_voltage_data[1];
            printf("The terminate voltage is: %d mV \n", terminate_voltage);

            printf("Trying to update the terminate voltage \n");
            delay(100);
            terminate_voltage = 3400;
            if(terminate_voltage > 3700)
                terminate_voltage = 3700;
            else if (terminate_voltage < 2500)
                terminate_voltage = 2500;           // #9
            printf("Trying to set new terminate voltage to: %d \n", terminate_voltage);
            to_hex_16(tmp, terminate_voltage);

            term_vol[0] = BQ27441_TERMINATE_VOLTAGE;//design_capacity_loc[0];
            term_vol[1] = (tmp[0] - '0')*16 + (tmp[1] - '0');
            term_vol[2] = (tmp[2] - '0')*16 + (tmp[3] - '0');

            I2cSendData(BQ27441_ADDR, term_vol, 3);
            delay(1000);

            /* Finishing up with the configuration */
            cksum = checksum(init_data);                                                         // #10
            delay(1000);
            printf("New Checksum found is: %x \n", cksum);

            block_data_checksum[1] = cksum;                                                 // #11
            I2cSendData(BQ27441_ADDR, block_data_checksum, 2);
            delay(1000);
            I2cSendData(BQ27441_ADDR, soft_reset, 3);                                       // #12
            delay(500);
            //printf("Design Cap data 0: %x", data[72]);
            //printf("Design Cap data :1 %x", data[73]);
            //design_capacity = data[72]*16*16 + data[73];

            I2cSendData(BQ27441_ADDR, flag_data, 1);                                        // #13
            delay(500);
            I2cReadData(BQ27441_ADDR, flag_out, 1);
            delay(500);
            printf("The flag_out is: %x \n", flag_out[0]);

            if(!CHECK_BIT(flag_out[0], 4)) {
                printf("CFGUPDTE has been exited, configuration done. \n");
                I2cSendData(BQ27441_ADDR, seal_data, 1);                                // #14
                delay(5);
                printf("Gauge has been sealed and is ready for operation \n");
            }
        } else {
            printf("Cannot proceed with configuration. \n");
            printf("The CFGUPDATE MODE has not been enabled yet. \n");
        }
    }
};

#endif // BQ27441_H
