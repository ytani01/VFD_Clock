//
// FabLab Kannai VFD Clock
//
// (c) 2018 FabLab Kannai
//
// VersionStr: "AA.BB.CC" -- 修正する前にとりあえず「CC」をカウントアップしましょう！
//   【ルール】
//   * ソースを一カ所でも変更したら(誤字訂正も!)、「CC」をカウントアップ。
//   * 大きなバグ修正やちょっとした機能変更をしたら、「BB」をカウントアップして、「CC」を「00」に戻す。
//   * 機能や作りを大きく変更した場合は、「AA」カウントアップして、「BB.CC」を「00.00」に戻す。
//
String VersionStr = "03.02.01";
#define DISP_VERSION_MSEC 5000 // msec

#include <Wire.h>
#include "RTClib.h"

#define DISP_DELAY                   2 // msec
#define BLINK_INTERVAL             500 // msec
#define BLINK_ON_MSEC              350 // msec
#define BUTTON_DEBOUNCE            150 // msec
#define BUTTON_LONGPRESSED_MSEC   1000 // msec
#define BUTTON_REPEAT_MSEC         100 // msec

#define PIN_SCL   A5
#define PIN_SDA   A4
#define PIN_BUTTON_MODE   3
#define PIN_BUTTON_COUNT  4

uint8_t PinSeg[] = {
  6, 7, 8, 9, 10, 11, 12, A1
};
unsigned long SegN = sizeof(PinSeg) / sizeof(uint8_t);

uint8_t PinDigit[] = {
  2, A0, 13, 5, A3, A2
};
unsigned long DigitN = sizeof(PinDigit) / sizeof(uint8_t);
unsigned long DigitI = 0;

boolean Num0[] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, LOW };
boolean Num1[] = {LOW , HIGH, HIGH, LOW , LOW , LOW , LOW };
boolean Num2[] = {HIGH, HIGH, LOW , HIGH, HIGH, LOW , HIGH};
boolean Num3[] = {HIGH, HIGH, HIGH, HIGH, LOW , LOW , HIGH};
boolean Num4[] = {LOW , HIGH, HIGH, LOW , LOW , HIGH, HIGH};
boolean Num5[] = {HIGH, LOW , HIGH, HIGH, LOW , HIGH, HIGH};
boolean Num6[] = {HIGH, LOW , HIGH, HIGH, HIGH, HIGH, HIGH};
boolean Num7[] = {HIGH, HIGH, HIGH, LOW , LOW , LOW , LOW };
boolean Num8[] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};
boolean Num9[] = {HIGH, HIGH, HIGH, HIGH, LOW , HIGH, HIGH};
boolean NumClr[] = {LOW , LOW , LOW , LOW , LOW , LOW , LOW }; // clear

boolean *Num[] = {Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9, NumClr};
#define NUM_CLR 10

#define MODE_NORMAL          0
#define MODE_SETTIME_HOUR    1
#define MODE_SETTIME_MIN     2
#define MODE_SETTIME_SEC     3
#define MODE_DISP_VERSION    9
#define MODE_ERROR        0xff
uint8_t   Mode  = MODE_NORMAL;

RTC_DS1307  Rtc;
DateTime CurTime;
volatile boolean AdjustFlag = false;

struct button_t {
  unsigned int  pin;
  unsigned long press_start;
  boolean       long_pressed;
  boolean       repeat;
} ButtonMode, ButtonCount;

unsigned long Prev_millis = 0;

uint8_t dispBuffer[] = {0, 1, 2, 3, 4, 5};

//=======================================================================
void pciSetup(byte pin) {
  *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));   // enable pin
  PCIFR  |= bit (digitalPinToPCICRbit(pin));                    // clear any outstanding interrupt
  PCICR  |= bit (digitalPinToPCICRbit(pin));                    // enable interrupt for the group
}

ISR (PCINT2_vect) {
  unsigned long cur_msec = millis();

  if ( cur_msec - Prev_millis < BUTTON_DEBOUNCE ) {
    return;
  }
  Serial.println("ISR()");
  Prev_millis = cur_msec;

  if (digitalRead(PIN_BUTTON_MODE) == LOW) {
    Serial.println("PIN_BUTTON_MODE: LOW");
    ButtonMode.press_start = cur_msec;
    ButtonMode.long_pressed = false;
    ButtonMode.repeat = false;
    Serial.println("ButtonMode.press_start = " + String(ButtonMode.press_start));
    setMode();
  } else {
    Serial.println("PIN_BUTTON_MODE: HIGH");
    ButtonMode.press_start = 0;
    ButtonMode.long_pressed = false;
    ButtonMode.repeat = false;
  }

  if (digitalRead(PIN_BUTTON_COUNT) == LOW) {
    Serial.println("PIN_BUTTON_COUNT: LOW");
    ButtonCount.press_start = cur_msec;
    ButtonCount.long_pressed = false;
    ButtonCount.repeat = false;
    Serial.println("ButtonCount.press_start = " + String(ButtonCount.press_start));
    if ( Mode == MODE_SETTIME_HOUR || Mode == MODE_SETTIME_MIN || Mode == MODE_SETTIME_SEC ) {
      changeTime();
    }
  } else {
    Serial.println("PIN_BUTTON_COUNT: HIGH");
    ButtonCount.press_start = 0;
    ButtonCount.long_pressed = false;
    ButtonCount.repeat = false;
  }
}

void setMode() {
  if ( Mode == MODE_NORMAL ) {
    // Do nothing !!
  } else if ( Mode == MODE_SETTIME_HOUR ) {
    Mode = MODE_SETTIME_MIN;
    Serial.println("* MODE_SETTIME_MIN");
  } else if ( Mode == MODE_SETTIME_MIN ) {
    Mode = MODE_SETTIME_SEC;
    Serial.println("* MODE_SETTIME_SEC");
  } else if ( Mode == MODE_SETTIME_SEC ) {
    Mode = MODE_NORMAL;
    Serial.println("* MODE_NORMAL");
    AdjustFlag = true;
  } else { // MODE_DISP_VERSION
    Mode = MODE_NORMAL;
    Serial.println("* MODE_NORMAL");
  }
}

void changeTime() {
  uint8_t hour    = CurTime.hour();
  uint8_t minute  = CurTime.minute();
  uint8_t second  = CurTime.second();

  if ( Mode == MODE_SETTIME_HOUR ) {
    hour = (hour + 1) % 24;
  } else if ( Mode == MODE_SETTIME_MIN ) {
    minute = (minute + 1) % 60;
  } else if ( Mode == MODE_SETTIME_SEC ) {
    second = ((second / 10) * 10 + 10) % 60;
  }

  CurTime = DateTime(CurTime.year(), CurTime.month(), CurTime.day(), hour, minute, second);
}

void checkButton(unsigned long cur_msec, struct button_t *button) {
  if ( button->press_start == 0 ) {
    button->long_pressed = false;
    button->repeat = false;
    return;
  }

  // press_start > 0 ... pressed before
  if ( digitalRead(button->pin) == HIGH ) {
    // released
    button->press_start = 0;
    button->long_pressed = false;
    button->repeat = false;
    return;
  }

  // pin: LOW ... pressed now
  if ( button->long_pressed ) {
    if ( ! button->repeat ) {
      if ( cur_msec - button->press_start > BUTTON_REPEAT_MSEC ) {
        button->repeat = true;
        Serial.println("Repeat: true");
        button->press_start = cur_msec;
      }
    }
  } else {
    if ( cur_msec - button->press_start > BUTTON_LONGPRESSED_MSEC ) {
      button->long_pressed = true;
      Serial.println("LongPressed: true");
      button->repeat = false;
      button->press_start = cur_msec;
    }
  }
}

//
// Display one digit
//
void dispOneDigit(uint8_t digit, uint8_t num, boolean dp) {
  for (int i = 0; i < DigitN; i++) {
    digitalWrite(PinDigit[i], LOW);
  }
  if ( num == NUM_CLR ) {
    return;
  }
  for (int i = 0; i < SegN - 1; i++) {
    digitalWrite(PinSeg[i], Num[num][i]);
  }
  digitalWrite(PinSeg[SegN - 1], dp);
  digitalWrite(PinDigit[digit], HIGH);
}

//
// Display all digits
//
void dispAllDigit() {
  boolean dp;

  for (int i = 0; i < DigitN; i++) {
    dp = false;
    if ( (i == 1 || i ==  3) && dispBuffer[i] != NUM_CLR ) {
      dp = true;
    }
    dispOneDigit(i, dispBuffer[i], dp);
    delay(DISP_DELAY);
  }
}

boolean blinkOff() {
  if ( millis() % BLINK_INTERVAL > BLINK_ON_MSEC ) {
    return true;
  }
  return false;
}

void displayVFD() {
  if ( Mode == MODE_DISP_VERSION ) {
    if ( blinkOff() ) {
      for ( int i = 0; i < DigitN; i++ ) {
        dispBuffer[i] = NUM_CLR;
      }
    } else {
      dispBuffer[0] = VersionStr[0] - '0';
      dispBuffer[1] = VersionStr[1] - '0';
      dispBuffer[2] = VersionStr[3] - '0';
      dispBuffer[3] = VersionStr[4] - '0';
      dispBuffer[4] = VersionStr[6] - '0';
      dispBuffer[5] = VersionStr[7] - '0';
    }
  } else {
    dispBuffer[0] = CurTime.hour() / 10;
    if ( dispBuffer[0] == 0 ) {
      dispBuffer[0] = NUM_CLR;
    }
    dispBuffer[1] = CurTime.hour()   % 10;
    dispBuffer[2] = CurTime.minute() / 10;
    dispBuffer[3] = CurTime.minute() % 10;
    dispBuffer[4] = CurTime.second() / 10;
    dispBuffer[5] = CurTime.second() % 10;
  }

  if ( digitalRead(PIN_BUTTON_COUNT) == HIGH && blinkOff() ) {
    if ( Mode == MODE_SETTIME_HOUR ) {
      dispBuffer[0] = dispBuffer[0 + 1] = NUM_CLR;
    }
    if ( Mode == MODE_SETTIME_MIN ) {
      dispBuffer[2] = dispBuffer[2 + 1] = NUM_CLR;
    }
    if ( Mode == MODE_SETTIME_SEC ) {
      dispBuffer[4] = dispBuffer[4 + 1] = NUM_CLR;
    }
  }

  dispAllDigit();
}
//=======================================================================
void setup() {
  Serial.begin(115200);
  Serial.println("Version: " + VersionStr);

  Wire.begin();
  Rtc.begin();

  Serial.println("SegN = " + String(SegN));
  Serial.println("DigitN = " + String(DigitN));

  for (int i = 0; i < SegN; i++) {
    pinMode(PinSeg[i], OUTPUT);
  }
  for (int i = 0; i < DigitN; i++) {
    pinMode(PinDigit[i], OUTPUT);
  }
  pinMode(PIN_BUTTON_MODE, INPUT_PULLUP);
  pinMode(PIN_BUTTON_COUNT, INPUT_PULLUP);

  ButtonMode.pin = PIN_BUTTON_MODE;
  ButtonMode.press_start = 0;
  ButtonMode.long_pressed = false;
  ButtonMode.repeat = false;

  ButtonCount.pin = PIN_BUTTON_COUNT;
  ButtonMode.press_start = 0;
  ButtonMode.long_pressed = false;
  ButtonMode.repeat = false;

  Mode = MODE_DISP_VERSION;
  Serial.println("* MODE_DISP_VERSION");

  pciSetup(PIN_BUTTON_MODE);
  pciSetup(PIN_BUTTON_COUNT);
  EICRA |= bit (ISC11);
  sei();
} // setup()
//=======================================================================
void loop() {
  unsigned long cur_msec = millis();

  if ( Mode == MODE_DISP_VERSION && cur_msec > DISP_VERSION_MSEC ) {
    Mode = MODE_NORMAL;
    Serial.println("* MODE_NORMAL");
  }

  checkButton(cur_msec, &ButtonMode);
  checkButton(cur_msec, &ButtonCount);

  if ( Mode == MODE_NORMAL ) {
    if ( ButtonMode.long_pressed ) {
      Mode = MODE_SETTIME_HOUR;
      Serial.println("* MODE_SETTIME_HOUR");
      return;
    }
    if ( AdjustFlag ) {
      Rtc.adjust(CurTime);
      AdjustFlag = false;
    }
    CurTime = Rtc.now();
  } else if ( Mode == MODE_SETTIME_HOUR || Mode == MODE_SETTIME_MIN || Mode == MODE_SETTIME_SEC ) {
    if ( ButtonCount.repeat ) {
      changeTime();
      ButtonCount.press_start = cur_msec;
      ButtonCount.repeat = false;
    }
  }

  displayVFD();
} // loop()
