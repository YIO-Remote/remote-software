#include "drv2605.h"
#include "../notifications.h"

drv2605::drv2605()
{
#ifdef __arm__
    if (init()) {
        selectLibrary(1);
        setMode(DRV2605_MODE_INTTRIG);
    } else {
        Notifications::getInstance()->add(true,tr("Cannot initialize the haptic motor. Please restart the remote."));
    }
#endif
}

void drv2605::playEffect(QString effect)
{
#ifdef __arm__
    if (effect == "click") {
        setWaveform(0, 2);
        go();
    }
    if (effect == "bump") {
        setWaveform(0, 26);
        go();
    }
    if (effect == "press") {
        setWaveform(0, 111);
        go();
    }
    if (effect == "buzz") {
        setWaveform(0, 48);
        go();
    }
#endif
}

bool drv2605::init()
{
#ifdef __arm__
    wiringPiSetup();
    bus = wiringPiI2CSetupInterface("/dev/i2c-3", DRV2605_ADDR);

//    int id = readRegister8(DRV2605_REG_STATUS);

    writeRegister8(DRV2605_REG_MODE, 0x00); // out of standby

//    writeRegister8(DRV2605_REG_RTPIN, 0x00); // no real-time-playback

//    writeRegister8(DRV2605_REG_WAVESEQ1, 1); // strong click
//    writeRegister8(DRV2605_REG_WAVESEQ2, 0); // end sequence

//    writeRegister8(DRV2605_REG_OVERDRIVE, 0); // no overdrive

//    writeRegister8(DRV2605_REG_SUSTAINPOS, 0);
//    writeRegister8(DRV2605_REG_SUSTAINNEG, 0);
//    writeRegister8(DRV2605_REG_BREAK, 0);
//    writeRegister8(DRV2605_REG_AUDIOMAX, 0x64);

    // turn off N_ERM_LRA
    writeRegister8(DRV2605_REG_FEEDBACK, readRegister8(DRV2605_REG_FEEDBACK) & 0x7F);
    // turn on ERM_OPEN_LOOP
    writeRegister8(DRV2605_REG_CONTROL3, readRegister8(DRV2605_REG_CONTROL3) | 0x20);

    return true;
#endif
}
#ifdef __arm__
void drv2605::setWaveform(uint8_t slot, uint8_t w)
{
    writeRegister8(DRV2605_REG_WAVESEQ1+slot, w);
}

void drv2605::selectLibrary(uint8_t lib) {
    writeRegister8(DRV2605_REG_LIBRARY, lib);
}

void drv2605::go() {
    writeRegister8(DRV2605_REG_GO, 1);
}

void drv2605::stop() {
    writeRegister8(DRV2605_REG_GO, 0);
}

void drv2605::setMode(uint8_t mode) {
    writeRegister8(DRV2605_REG_MODE, mode);
}

void drv2605::setRealtimeValue(uint8_t rtp) {
    writeRegister8(DRV2605_REG_RTPIN, rtp);
}

int drv2605::readRegister8(uint8_t reg)
{
    int x;
    x = wiringPiI2CReadReg8(bus, reg);

    return x;
}

void drv2605::writeRegister8(uint8_t reg, uint8_t val) {
    wiringPiI2CWriteReg8(bus, reg, val);
}
#endif
