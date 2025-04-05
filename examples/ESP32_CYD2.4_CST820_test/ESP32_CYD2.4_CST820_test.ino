#include <CYD_CST820.h>
#include <Wire.h>
#define I2C_TOUCH_ADDR 0x15
#include <SPI.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

#define I2C_SDA 33
#define I2C_SCL 32
#define TP_RST 25
#define TP_INT 21
CYD_CST820 ctp = CYD_CST820();

CYD_CST820 touchscreen(I2C_SDA, I2C_SCL, TP_RST, TP_INT);

#define SCREEN_WIDTH 320 //szélesség
#define SCREEN_HEIGHT 240 // magasság
#define FONT_SIZE 2

// Touchscreen coordinates: (x, y) and pressure (z)
int x, y, z;

// Print Touchscreen info about X, Y and Pressure (Z) on the Serial Monitor
void printTouchToSerial(int touchX, int touchY, int touchZ) {
  Serial.print("X = ");
  Serial.print(touchX);
  Serial.print(" | Y = ");
  Serial.print(touchY);
  Serial.print(" | Pressure = ");
  Serial.print(touchZ);
  Serial.println();
}

// Print Touchscreen info about X, Y and Pressure (Z) on the TFT Display
void printTouchToDisplay(int touchX, int touchY, int touchZ) {
  // Clear TFT screen
  tft.fillScreen(TFT_WHITE);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);

  int centerX = SCREEN_WIDTH / 2;
  int textY = 80;

  String tempText = "X = " + String(touchX);
  tft.drawCentreString(tempText, centerX, textY, FONT_SIZE);

  textY += 20;
  tempText = "Y = " + String(touchY);
  tft.drawCentreString(tempText, centerX, textY, FONT_SIZE);

  textY += 20;
  tempText = "Pressure = " + String(touchZ);
  tft.drawCentreString(tempText, centerX, textY, FONT_SIZE);
}

void setup() {
  Serial.begin(115200);
  // Set the Touchscreen  rotation in landscape mode
  touchscreen.setRotation(1);

  // Start the tft display
  tft.init();
  // Set the TFT display rotation in landscape mode
  tft.setRotation(1);

  // Clear the screen before writing to it
  tft.fillScreen(TFT_WHITE);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);

  // Set X and Y coordinates for center of display
  int centerX = SCREEN_WIDTH / 2;
  int centerY = SCREEN_HEIGHT / 2;

  tft.drawCentreString("Hello, world!", centerX, 30, FONT_SIZE);
  tft.drawCentreString("Touch screen to test", centerX, centerY, FONT_SIZE);
  touchscreen.begin();
  digitalWrite(27, HIGH);
}

void loop() {
  bool touched;
  int a;
  uint8_t gesture;
  uint16_t touchX, touchY;
  touched = touchscreen.getTouch(&touchX, &touchY, &gesture);
  // Checks if Touchscreen was touched, and prints X, Y and Pressure (Z) info on the TFT display and Serial Monitor
  if (!touched) {
  } else {
    // Get Touchscreen points
    TS_Point p = touchscreen.getPoint();
    // Calibrate Touchscreen points with map function to the correct width and height
    x = map(p.x, 0, 320, 1, SCREEN_WIDTH);
    y = map(p.y, 0, 240, 1, SCREEN_HEIGHT);
    
    Serial.println((String) "Touch Data x: " + touchX + (String) ", y: " + touchY + (String) ", gesture: " + gesture);

    printTouchToSerial(x, y, z);
    printTouchToDisplay(x, y, z);
    /*Ha az x,y van használva akkor a setRotation(1) átfordítja a touch screen-t
       A touchX,touchY nyers adat,  kirajzoláshoz csak akkor fordítja át ha
       b = SCREEN_HEIGHT - touchX; b a touchX újra definiálva
    */
    delay(100);
  }

  /*int b;
    b = SCREEN_HEIGHT - touchX; //touchscreen fordítása 90fokkal
    tft.fillCircle(touchY, b, 3, TFT_BLACK);*/
  tft.fillCircle(y, x, 3, TFT_BLACK);
}
