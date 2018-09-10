// VFD Clock Game 1
// (c) 2018 FabLab Kannai
//
String VersionStr = "01.00.00";
#define DISP_VERSION_MSEC  3000  // msec

#include <Wire.h>
#include "Button.h"
#include "VFD.h"

#define DISP_DELAY                 2 // msec
#define BLINK_INTERVAL           500 // msec
#define BLINK_ON_MSEC            350 // msec
#define BUTTON_DEBOUNCE           10 // msec

#define PIN_SCL           A5
#define PIN_SDA           A4
#define PIN_BUTTON_MODE   3
#define PIN_BUTTON_SET    4

uint8_t PinSeg[]     = { 6, 7, 8, 9, 10, 11, 12, A1 };
uint8_t SegN         = sizeof(PinSeg) / sizeof(uint8_t);
boolean Val0[]       = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, LOW };
boolean Val1[]       = {LOW , HIGH, HIGH, LOW , LOW , LOW , LOW };
boolean Val2[]       = {HIGH, HIGH, LOW , HIGH, HIGH, LOW , HIGH};
boolean Val3[]       = {HIGH, HIGH, HIGH, HIGH, LOW , LOW , HIGH};
boolean Val4[]       = {LOW , HIGH, HIGH, LOW , LOW , HIGH, HIGH};
boolean Val5[]       = {HIGH, LOW , HIGH, HIGH, LOW , HIGH, HIGH};
boolean Val6[]       = {HIGH, LOW , HIGH, HIGH, HIGH, HIGH, HIGH};
boolean Val7[]       = {HIGH, HIGH, HIGH, LOW , LOW , LOW , LOW };
boolean Val8[]       = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};
boolean Val9[]       = {HIGH, HIGH, HIGH, HIGH, LOW , HIGH, HIGH};
boolean ValClr[]     = {LOW , LOW , LOW , LOW , LOW , LOW , LOW }; // clear
boolean ValBarHigh[] = {HIGH, LOW , LOW , LOW , LOW , LOW , LOW };
boolean ValBarMid[]  = {LOW , LOW , LOW , LOW , LOW , LOW , HIGH};
boolean ValBarLow[]  = {LOW , LOW , LOW , HIGH, LOW , LOW , LOW };
boolean *Val[]       = {Val0, Val1, Val2, Val3, Val4, Val5, Val6, Val7, Val8, Val9, ValClr, ValBarHigh, ValBarMid, ValBarLow};
#define VAL_CLR      10
#define VAL_BAR_HIGH 11
#define VAL_BAR_MID  12
#define VAL_BAR_LOW  13

uint8_t PinDigit[]   = { 2, A0, 13, 5, A3, A2 };
uint8_t DigitN       = sizeof(PinDigit) / sizeof(uint8_t);
uint8_t DigitI       = 0;
uint8_t DispBuf[]    = {1, 2, 3, 4, 5, 6};

VFD Vfd = VFD(PinSeg, PinDigit, Val);

Button ButtonMode, ButtonSet;

#define MODE_DISP_VERSION         0x00
#define MODE_NORMAL               0x10
uint8_t       Mode = MODE_DISP_VERSION;

unsigned long CurMsec  = 0;
unsigned long PrevMsec = 0;

unsigned long Count = 0;
//=========================================================
ISR (PCINT2_vect) {
  unsigned long cur_msec = millis();
  static unsigned long prev_msec = 0;

  if ( cur_msec - prev_msec < BUTTON_DEBOUNCE ) {
    return;
  }
  Serial.println("ISR()");
  prev_msec = cur_msec;

  if (ButtonMode.get()) {
    if ( ButtonMode.value() == LOW ) {
      Count++;
      Serial.println("Count = " + String(Count));
    }
  }
  if (ButtonSet.get()) {
    if (ButtonSet.value() == LOW ) {
      Count--;
      Serial.println("Count = " + String(Count));
    }
  }
}
//---------------------------------------------------------
void displayVersion() {
  DispBuf[0] = VersionStr[0] - '0';
  DispBuf[1] = VersionStr[1] - '0';
  DispBuf[2] = VersionStr[3] - '0';
  DispBuf[3] = VersionStr[4] - '0';
  DispBuf[4] = VersionStr[6] - '0';
  DispBuf[5] = VersionStr[7] - '0';

  if ( blinkOff() ) {
    clearBuf();
  }
}

void displayCount() {
  DispBuf[5] = Count % 10;
  DispBuf[4] = Count / 10 % 10;
  DispBuf[3] = Count / 100 % 10;
  DispBuf[2] = Count / 1000 % 10;
  DispBuf[1] = Count / 10000 % 10;
  DispBuf[0] = Count / 100000 % 10;
}

boolean blinkOff() {
  if ( CurMsec % BLINK_INTERVAL > BLINK_ON_MSEC ) {
    return true;
  }
  return false;
}
void clearBuf() {
  for (int i = 0; i < DigitN; i++) {
    DispBuf[i] = VAL_CLR;
  }
}
void dispOneDigit() {
  boolean dp = false;

  if ( DispBuf[DigitI] != VAL_CLR ) {
    for (int i = 0; i < SegN - 1; i++) {
      digitalWrite(PinSeg[i], Val[DispBuf[DigitI]][i]);
    }
    digitalWrite(PinSeg[SegN - 1], dp);
    digitalWrite(PinDigit[DigitI], HIGH);

    // delay(DISP_DELAY);
  }

  delay(DISP_DELAY);

  digitalWrite(PinDigit[DigitI], LOW);
  DigitI = (DigitI + 1) % DigitN;
}
void displayVFD() {
  if ( Mode == MODE_DISP_VERSION ) {
    if ( CurMsec > DISP_VERSION_MSEC ) {
      Mode = MODE_NORMAL;
    } else {
      displayVersion();
    }
  }

  if ( Mode == MODE_NORMAL ) {
    displayCount();
  }

  dispOneDigit();
}
//---------------------------------------------------------
void setup() {
  Serial.begin(115200);
  Serial.println("Version: " + VersionStr);

  Wire.begin();

  for (int i = 0; i < SegN; i++) {
    pinMode(PinSeg[i], OUTPUT);
  }
  for (int i = 0; i < DigitN; i++) {
    pinMode(PinDigit[i], OUTPUT);
    DispBuf[i] = VAL_CLR;
  }
  Mode = MODE_DISP_VERSION;

  ButtonMode.init(PIN_BUTTON_MODE, "[MODE]");
  ButtonSet.init(PIN_BUTTON_SET,  "[SET]");
  
//  pinMode(PIN_BUTTON_MODE, INPUT_PULLUP);
//  pinMode(PIN_BUTTON_SET,  INPUT_PULLUP);
//  initButton(&ButtonMode, PIN_BUTTON_MODE, "[MODE]");
//  initButton(&ButtonSet,  PIN_BUTTON_SET,  "[SET]");
  sei();
}

void loop() {
  CurMsec = millis();

  if (ButtonMode.get()) {
    Serial.println(ButtonMode.name() + ": " + String(ButtonMode.count()) + ", " + String(ButtonMode.long_pressed()) + ", " + String(ButtonMode.repeat()));
    Count++;
  }
  if (ButtonSet.get()) {
    Count--;
  }

  displayVFD();
}
