#ifndef CYD_CST820_LIBRARY
#define CYD_CST820_LIBRARY

#include <Wire.h>
#include "Arduino.h"
#include <Adafruit_I2CDevice.h>

#define I2C_ADDR_CST820 0x15
#define MAX_TOUCHES 5 //!< Maximum number of touches to detect
//#define CST8XX_DEFAULT_ADDR 0x15
#define CST8XX_REG_NUMTOUCHES 0x02 //!< Number of touch points
#define CST820_CHIPID 0xB7  //!< Chip selecting

//uint8_t rotation = 0;

//手势
enum GESTURE
{
  None = 0x00,       //无手势
  SlideDown = 0x01,  //向下滑动
  SlideUp = 0x02,    //向上滑动
  SlideLeft = 0x03,  //向左滑动
  SlideRight = 0x04, //向右滑动
  SingleTap = 0x05,  //单击
  DoubleTap = 0x0B,  //双击
  LongPress = 0x0C   //长按
};

enum Events {
  PRESS = 0, ///< Press Event Detected
  RELEASE,   ///< Release Event Detected
  TOUCHING,  ///< Touching Detected
  NONE       ///< Not Touching Detected
};

const char *const events_name[] = {"PRESS", "RELEASE", "TOUCHING",
                                   "NONE"
                                  }; //!< Event names to print


class TS_Point {
  public:
    TS_Point(void);
    TS_Point(int16_t x, int16_t y, int16_t z, uint8_t id = 0,
             enum Events event = NONE);

    bool operator==(TS_Point);
    bool operator!=(TS_Point);

    int16_t x; /*!< X coordinate */
    int16_t y; /*!< Y coordinate */
    int16_t z; /*!< Z coordinate (often used for pressure) */

    uint8_t id;        /*!< Touch ID */
    enum Events event; /*!< Event type */
};
/**************************************************************************/
/*!
    @brief  CST820 I2C CTP controller driver
*/
/**************************************************************************/


class CYD_CST820 {
  public:
    CYD_CST820(int8_t sda_pin = -1, int8_t scl_pin = -1, int8_t rst_pin = -1, int8_t int_pin = -1);
    //bool begin(TwoWire *theWire = &Wire, uint8_t i2c_addr = I2C_ADDR_CST820);
    void begin(void);
    bool getTouch(uint16_t *x, uint16_t *y, uint8_t *gesture);
    uint8_t touched(void);
    TS_Point getPoint(uint8_t n = 0);
    void setRotation(uint8_t n);

  private:
    Adafruit_I2CDevice *i2c_dev = NULL; ///< Pointer to I2C bus interface
    void writeRegister8(uint8_t reg, uint8_t val);
    uint8_t readRegister8(uint8_t reg);
    uint16_t readRegister16(uint8_t reg);
    int8_t _sda, _scl, _rst, _int;
    uint8_t i2c_read(uint8_t addr);
    uint8_t i2c_read_continuous(uint8_t addr, uint8_t *data, uint32_t length);
    void i2c_write(uint8_t addr, uint8_t data);
    uint8_t i2c_write_continuous(uint8_t addr, const uint8_t *data, uint32_t length);
    void readData(void);
    uint8_t touches;
    uint16_t touchX[MAX_TOUCHES], touchY[MAX_TOUCHES];
    uint8_t touchID[MAX_TOUCHES];
    uint8_t rotation = 0;
    enum Events event[MAX_TOUCHES];
};


#endif

