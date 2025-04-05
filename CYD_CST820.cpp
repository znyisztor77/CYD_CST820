#include "CYD_CST820.h"

//CYD_CST820::CYD_CST820() { touches = 0; }

CYD_CST820::CYD_CST820(int8_t sda_pin, int8_t scl_pin, int8_t rst_pin, int8_t int_pin)
{
  _sda = sda_pin;
  _scl = scl_pin;
  _rst = rst_pin;
  _int = int_pin;
}

void CYD_CST820::begin(void)
{
  // Initialize I2C
  if (_sda != -1 && _scl != -1)
  {
    Wire.begin(_sda, _scl);
  }
  else
  {
    Wire.begin();
  }

  // Int Pin Configuration
  if (_int != -1)
  {
    pinMode(_int, OUTPUT);
    digitalWrite(_int, HIGH); //高电平
    delay(1);
    digitalWrite(_int, LOW); //低电平
    delay(1);
  }

  // Reset Pin Configuration
  if (_rst != -1)
  {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, LOW);
    delay(10);
    digitalWrite(_rst, HIGH);
    delay(300);
  }

  // Initialize Touch
  i2c_write(0xFE, 0XFF); //禁止自动进入低功耗模式。
}

bool CYD_CST820::getTouch(uint16_t *x, uint16_t *y, uint8_t *gesture)
{
  bool FingerIndex = false;
  FingerIndex = (bool)i2c_read(0x02);

  *gesture = i2c_read(0x01);
  if (!(*gesture == SlideUp || *gesture == SlideDown))
  {
    *gesture = None;
  }

  uint8_t data[4];
  i2c_read_continuous(0x03, data, 4);
  *x = ((data[0] & 0x0f) << 8) | data[1];
  *y = ((data[2] & 0x0f) << 8) | data[3];


  return FingerIndex;
}
/*void CYD_CST820::setRotation( uint8_t rotation){
  switch (rotation) {
      case 0:
      touchX = *x;
      touchY = *y;
      break;
      case 1:
      touchX = 240-*x;
      touchY = *y;
      break;
      default:
      touchX = *x;
      touchY = *y;
  }*/

void CYD_CST820::setRotation(uint8_t n) {
  rotation = n % 4;
}
uint8_t CYD_CST820::touched(void) {
  uint8_t n = readRegister8(CST8XX_REG_NUMTOUCHES);

  if (n > MAX_TOUCHES) {
    n = 0;
  }

  return n;
}

uint8_t CYD_CST820::i2c_read(uint8_t addr)
{
  uint8_t rdData;
  uint8_t rdDataCount;
  do
  {
    Wire.beginTransmission(I2C_ADDR_CST820);
    Wire.write(addr);
    Wire.endTransmission(false); // Restart
    rdDataCount = Wire.requestFrom(I2C_ADDR_CST820, 1);
  } while (rdDataCount == 0);
  while (Wire.available())
  {
    rdData = Wire.read();
  }
  return rdData;
}

uint8_t CYD_CST820::i2c_read_continuous(uint8_t addr, uint8_t *data, uint32_t length)
{
  Wire.beginTransmission(I2C_ADDR_CST820);
  Wire.write(addr);
  if ( Wire.endTransmission(true))return -1;
  Wire.requestFrom(I2C_ADDR_CST820, length);
  for (int i = 0; i < length; i++) {
    *data++ = Wire.read();
  }
  return 0;
}

void CYD_CST820::i2c_write(uint8_t addr, uint8_t data)
{
  Wire.beginTransmission(I2C_ADDR_CST820);
  Wire.write(addr);
  Wire.write(data);
  Wire.endTransmission();
}

uint8_t CYD_CST820::i2c_write_continuous(uint8_t addr, const uint8_t *data, uint32_t length)
{
  Wire.beginTransmission(I2C_ADDR_CST820);
  Wire.write(addr);
  for (int i = 0; i < length; i++) {
    Wire.write(*data++);
  }
  if ( Wire.endTransmission(true))return -1;
  return 0;
}

TS_Point CYD_CST820::getPoint(uint8_t n) {
  readData();
  if ((touches == 0) || (n > 1)) {
    return TS_Point(0, 0, 0);
  } else {
    //return TS_Point(touchX[n], touchY[n], 1, touchID[n], event[n]);
    int16_t x = touchX[n];
    int16_t y = touchY[n];

    switch (rotation) {
      case 0:
        break;
      case 1:
        x = 240 - x; // vagy a kijelző méreteid szerint
        break;
      case 2:
        x = 240 - x;
        y = 320 - y;
        break;
      case 3:
        y = 320 - y;
        break;
    }

    return TS_Point(x, y, 1, touchID[n], event[n]);

  }
}



uint8_t CYD_CST820::readRegister8(uint8_t reg) {
  uint8_t buffer[1] = {reg};
  i2c_dev->write_then_read(buffer, 1, buffer, 1);
  return buffer[0];
}

void CYD_CST820::writeRegister8(uint8_t reg, uint8_t val) {
  uint8_t buffer[2] = {reg, val};
  i2c_dev->write(buffer, 2);
}

uint16_t CYD_CST820::readRegister16(uint8_t reg) {
  uint8_t buffer[2] = {reg};
  i2c_dev->write_then_read(buffer, 2, buffer, 1);
  return buffer[1] << 8 | buffer[0];
}

void CYD_CST820::readData(void) {
  uint8_t data[11];  // Az érintési adatokat tároló tömb
  if (i2c_read_continuous(0x00, data, 11) != 0) {
    touches = 0;
    return;
  }

  touches = data[2] & 0x0F;  // Az érintések számának kiolvasása

  for (uint8_t i = 0; i < touches; i++) {
    touchX[i] = ((data[3 + i * 6] & 0x0F) << 8) | data[4 + i * 6];
    touchY[i] = ((data[5 + i * 6] & 0x0F) << 8) | data[6 + i * 6];
    touchID[i] = data[5 + i * 6] >> 4;
    event[i] = (Events)(data[3 + i * 6] >> 6);
  }
}
/**************************************************************************/
/*!
    @brief  Instantiates a new CST8XX class with x, y and z set to 0 by default
*/
/**************************************************************************/
TS_Point::TS_Point(void) {
  x = y = z = 0;
}

/**************************************************************************/
/*!
    @brief  Instantiates a new CST8XX class with x, y and z set by params.
    @param  _x The X coordinate
    @param  _y The Y coordinate
    @param  _z The Z coordinate
    @param  _id The touch ID
    @param  _event The event type
*/
/**************************************************************************/

TS_Point::TS_Point(int16_t _x, int16_t _y, int16_t _z, uint8_t _id,
                   enum Events _event) {
  x = _x;
  y = _y;
  z = _z;
  id = _id;
  event = _event;
}

/**************************************************************************/
/*!
    @brief  Simple == comparator for two CST_TS_Point objects
    @returns True if x, y, z, and id are the same for both points, False
   otherwise.
*/
/**************************************************************************/
bool TS_Point::operator==(TS_Point p1) {
  return ((p1.x == x) && (p1.y == y) && (p1.z == z) && (p1.id == id));
}

/**************************************************************************/
/*!
    @brief  Simple != comparator for two CST_TS_Point objects
    @returns False if x, y, z, and id are the same for both points, True
   otherwise.
*/
/**************************************************************************/
bool TS_Point::operator!=(TS_Point p1) {
  return ((p1.x != x) || (p1.y != y) || (p1.z != z) || p1.id != id);
}
