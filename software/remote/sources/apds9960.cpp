#include "apds9960.h"

APDS9960::APDS9960() {}

APDS9960::~APDS9960() {}

void APDS9960::enable(bool en)
{
    _enable.PON = en;
    wiringPiI2CWriteReg8(_fd, APDS9960_ENABLE,_enable.get());
}

bool APDS9960::begin(uint16_t iTimeMS, apds9960AGain_t aGain)
{
    _fd = wiringPiI2CSetupInterface("/dev/i2c-3", APDS9960_ADDRESS);
    if(_fd == -1) {
        return false;
    }

    uint8_t x = uint8_t(wiringPiI2CReadReg8(_fd, APDS9960_ID));
    if (x != 0xAB) {
        return false;
    }

    /* Set default integration time and gain */
    setADCIntegrationTime(iTimeMS);
    setADCGain(aGain);

    // disable everything to start
    enableGesture(false);
    enableProximity(false);
    enableColor(false);

    disableColorInterrupt();
    disableProximityInterrupt();
    clearInterrupt();

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
    _gpulse.GPULSE = 9; // 10 pulses
    wiringPiI2CWriteReg8(_fd, APDS9960_GPULSE, _gpulse.get());

    return true;
}

void APDS9960::setADCIntegrationTime(uint16_t iTimeMS) {
  float temp;

  // convert ms into 2.78ms increments
  temp = iTimeMS;
  temp /= 2.78;
  temp = 256 - temp;
  if (temp > 255)
    temp = 255;
  if (temp < 0)
    temp = 0;

  /* Update the timing register */
  wiringPiI2CWriteReg8(_fd, APDS9960_ATIME, (uint8_t)temp);
}

float APDS9960::getADCIntegrationTime() {
  float temp;

  temp = float(wiringPiI2CReadReg8(_fd, APDS9960_ATIME));

  // convert to units of 2.78 ms
  temp = 256 - temp;
  temp *= 2.78;
  return temp;
}

void APDS9960::setADCGain(apds9960AGain_t aGain) {
  _control.AGAIN = aGain;

  /* Update the timing register */
  wiringPiI2CWriteReg8(_fd, APDS9960_CONTROL, _control.get());
}

apds9960AGain_t APDS9960::getADCGain() {
  return apds9960AGain_t((wiringPiI2CReadReg8(_fd, APDS9960_CONTROL) & 0x03));
}

void APDS9960::setProxGain(apds9960PGain_t pGain) {
  _control.PGAIN = pGain;

  /* Update the timing register */
  wiringPiI2CWriteReg8(_fd, APDS9960_CONTROL, _control.get());
}

apds9960PGain_t APDS9960::getProxGain() {
  return apds9960PGain_t((wiringPiI2CReadReg8(_fd, APDS9960_CONTROL) & 0x0C));
}

void APDS9960::setProxPulse(apds9960PPulseLen_t pLen, uint8_t pulses) {
  if (pulses < 1)
    pulses = 1;
  if (pulses > 64)
    pulses = 64;
  pulses--;

  _ppulse.PPLEN = pLen;
  _ppulse.PPULSE = pulses;

  wiringPiI2CWriteReg8(_fd, APDS9960_PPULSE, _ppulse.get());
}

void APDS9960::enableProximity(bool en) {
  _enable.PEN = en;

  wiringPiI2CWriteReg8(_fd, APDS9960_ENABLE, _enable.get());
}

void APDS9960::enableProximityInterrupt() {
  _enable.PIEN = 1;
  wiringPiI2CWriteReg8(_fd, APDS9960_ENABLE, _enable.get());
  clearInterrupt();
}

void APDS9960::disableProximityInterrupt() {
  _enable.PIEN = 0;
  wiringPiI2CWriteReg8(_fd, APDS9960_ENABLE, _enable.get());
}

void APDS9960::setProximityInterruptThreshold(uint8_t low,
                                                       uint8_t high,
                                                       uint8_t persistance) {
  wiringPiI2CWriteReg8(_fd, APDS9960_PILT, low);
  wiringPiI2CWriteReg8(_fd, APDS9960_PIHT, high);

  if (persistance > 7)
    persistance = 7;
  _pers.PPERS = persistance;
  wiringPiI2CWriteReg8(_fd, APDS9960_PERS, _pers.get());
}

bool APDS9960::getProximityInterrupt() {
  _status.set(uint8_t(wiringPiI2CReadReg8(_fd, APDS9960_STATUS)));
  return _status.PINT;
}

uint8_t APDS9960::readProximity() { return uint8_t(wiringPiI2CReadReg8(_fd, APDS9960_PDATA)); }

bool APDS9960::gestureValid() {
  _gstatus.set(uint8_t(wiringPiI2CReadReg8(_fd, APDS9960_GSTATUS)));
  return _gstatus.GVALID;
}

void APDS9960::setGestureDimensions(uint8_t dims) {
  _gconf3.GDIMS = dims;
  wiringPiI2CWriteReg8(_fd, APDS9960_GCONF3, _gconf3.get());
}

void APDS9960::setGestureFIFOThreshold(uint8_t thresh) {
  _gconf1.GFIFOTH = thresh;
  wiringPiI2CWriteReg8(_fd, APDS9960_GCONF1, _gconf1.get());
}

void APDS9960::setGestureGain(uint8_t gain) {
  _gconf2.GGAIN = gain;
  wiringPiI2CWriteReg8(_fd, APDS9960_GCONF2, _gconf2.get());
}

void APDS9960::setGestureProximityThreshold(uint8_t thresh) {
  wiringPiI2CWriteReg8(_fd, APDS9960_GPENTH, thresh);
}

void APDS9960::setGestureOffset(uint8_t offset_up, uint8_t offset_down,
                                         uint8_t offset_left,
                                         uint8_t offset_right) {
  wiringPiI2CWriteReg8(_fd, APDS9960_GOFFSET_U, offset_up);
  wiringPiI2CWriteReg8(_fd, APDS9960_GOFFSET_D, offset_down);
  wiringPiI2CWriteReg8(_fd, APDS9960_GOFFSET_L, offset_left);
  wiringPiI2CWriteReg8(_fd, APDS9960_GOFFSET_R, offset_right);
}

void APDS9960::enableGesture(bool en) {
  if (!en) {
    _gconf4.GMODE = 0;
    wiringPiI2CWriteReg8(_fd, APDS9960_GCONF4, _gconf4.get());
  }
  _enable.GEN = en;
  wiringPiI2CWriteReg8(_fd, APDS9960_ENABLE, _enable.get());
  resetCounts();
}

void APDS9960::resetCounts() {
  gestCnt = 0;
  UCount = 0;
  DCount = 0;
  LCount = 0;
  RCount = 0;
}

void APDS9960::setLED(apds9960LedDrive_t drive,
                               apds9960LedBoost_t boost) {
  // set BOOST
  _config2.LED_BOOST = boost;
  wiringPiI2CWriteReg8(_fd, APDS9960_CONFIG2, _config2.get());

  _control.LDRIVE = drive;
  wiringPiI2CWriteReg8(_fd, APDS9960_CONTROL, _control.get());
}

void APDS9960::enableColor(bool en) {
  _enable.AEN = en;
  wiringPiI2CWriteReg8(_fd, APDS9960_ENABLE, _enable.get());
}

bool APDS9960::colorDataReady() {
  _status.set(uint8_t(wiringPiI2CReadReg8(_fd, APDS9960_STATUS)));
  return _status.AVALID;
}

void APDS9960::getColorData(uint16_t *r, uint16_t *g, uint16_t *b,
                                     uint16_t *c) {

  *c = uint16_t(wiringPiI2CReadReg16(_fd, APDS9960_CDATAL));
  *r = uint16_t(wiringPiI2CReadReg16(_fd, APDS9960_RDATAL));
  *g = uint16_t(wiringPiI2CReadReg16(_fd, APDS9960_GDATAL));
  *b = uint16_t(wiringPiI2CReadReg16(_fd, APDS9960_BDATAL));
}

void APDS9960::getAmbientLight(uint16_t &val)
{
    uint16_t val_byte;
    val = 0;

    val = uint16_t(wiringPiI2CReadReg8(_fd, APDS9960_CDATAL));

    val_byte = uint16_t(wiringPiI2CReadReg8(_fd, APDS9960_CDATAH));

    val = val + ((uint16_t)val_byte << 8);
}

uint16_t APDS9960::calculateLux(uint16_t r, uint16_t g, uint16_t b) {
  float illuminance;

  /* This only uses RGB ... how can we integrate clear or calculate lux */
  /* based exclusively on clear since this might be more reliable?      */
  illuminance = (-0.32466F * r) + (1.57837F * g) + (-0.73191F * b);

  return uint16_t(illuminance);
}

void APDS9960::enableColorInterrupt() {
  _enable.AIEN = 1;
  wiringPiI2CWriteReg8(_fd, APDS9960_ENABLE, _enable.get());
}

void APDS9960::disableColorInterrupt() {
  _enable.AIEN = 0;
  wiringPiI2CWriteReg8(_fd, APDS9960_ENABLE, _enable.get());
}

void APDS9960::clearInterrupt() {
//  write(APDS9960_AICLEAR, NULL, 0);
  wiringPiI2CWrite(_fd, APDS9960_AICLEAR);
  wiringPiI2CWrite(_fd, APDS9960_PICLEAR);
  wiringPiI2CWrite(_fd, APDS9960_CICLEAR);
}

void APDS9960::setIntLimits(uint16_t low, uint16_t high) {
  wiringPiI2CWriteReg8(_fd, APDS9960_AILTIL, low & 0xFF);
  wiringPiI2CWriteReg8(_fd, APDS9960_AILTH, low >> 8);
  wiringPiI2CWriteReg8(_fd, APDS9960_AIHTL, high & 0xFF);
  wiringPiI2CWriteReg8(_fd, APDS9960_AIHTH, high >> 8);
}
