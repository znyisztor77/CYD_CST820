# ESP32 2.4inch Cheap Yellow Display Board CST820 capacitive touchscreen driver

This library is for the ESP32 2.4 inch CHEAP YELLOW DISPLAY (ESP32-2432S024C) development board. If your touchscreen is capacitive, most of the web examples will not work, because most tutorials are for resistive touchscreens. This library allows you to test a 2.4 inch capacitive touchscreen driven by the CST820 driver with the included example program. 
Currently, only the rotation of the touchscreen works (setRotation()), gesture control is not developed. The User_Setup.h extension is included, which must be copied into the TFT_eSPI library. Good luck!
