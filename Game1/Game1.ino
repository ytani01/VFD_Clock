// VFD Clock Game 1
//
// (c) 2018 FabLab Kannai
//
String VersionStr = "01.00.00";
#define DSIP_VERSION_MSEC  5000  // msec

#include <Wire.h>

#define DISP_DELAY                 2 // msec
#define BLINK_INTERVAL           500 // msec
#define BLINK_ON_MSEC            350 // msec
#define BUTTON_DEBOUNCE          150 // msec
#define BUTTON_LONGPRESSED_MSEC 1000 // msec
#define BUTTON_REPEAT_MSEC       100 // msec

#define PIN_SCL           A5
#define PIN_SDA           A4
#define PIN_BUTTON_MODE   3
#define PIN_BUTTON_SET    4

uint8_t PinSeg[] = { 6, 7, 8, 9, 10, 11, 12, A1 };
uint8_t SegN      = sizeof(PinSeg) / sizeof(uint8_t);

uint8_t PinDigit[] = { 2, A0, 13, 5, A3, A2 };
uint8_t DigitN    = sizeof(PinDigit) / sizeof(uint8_t);
uint8_t DigitI    = 0;

#define MODE_DISP_VERSION         0x00

uint8_t       Mode = MODE_DISP_VERSION;

unsigned long CurMsec = 0;

//---------------------------------------------------------
void setup() {
  CurMsec = millis();
}

void loop() {
  CurMsec = millis();
  
}
