//
// FabLab Kannai VFD Clock
//
// (c) 2018 FabLab Kannai
//
// VERSION: "AA.BB.CC" -- 修正する前にとりあえず「CC」をカウントアップしましょう！
//   * ソースを一カ所でも変更したら(誤字訂正も!)、「CC」をカウントアップ。
//   * 大きなバグ修正やちょっとした機能変更をしたら、「BB」をカウントアップして、「CC」を「00」に戻す。
//   * 機能や作りを大きく変更した場合は、「AA」カウントアップして、「BB.CC」を「00.00」に戻す。
// 
#define VERSION  "03.01.00"

#include "RTClib.h"
// #include <FlexiTimer2.h>
#include <Wire.h>

#if 1
#define DEGUB
#endif

#define DISP_DELAY  2 // msec

#define PIN_SCL   A5
#define PIN_SDA   A4

#define PIN_BUTTON_MODE   3
#define PIN_BUTTON_COUNT  4

#define BUTTON_DEBOUNCE           150  // msec
#define BUTTON_LONGPRESSED_MSEC   1000 // msec
#define BUTTON_REPEAT_MSEC        180  // msec

#define BLINK_INTERVAL            500 // msec
#define BLINK_ON_MSEC             300 // msec

unsigned long ButtonMode_PressStart   = 0; // msec
boolean       ButtonMode_LongPressed  = false;
boolean       ButtonMode_Repeat       = false;
unsigned long ButtonCount_PressStart  = 0; // msec
boolean       ButtonCount_LongPressed = false;
boolean       ButtonCount_Repeat      = false;

#define MODE_NORMAL         0
#define MODE_SETTIME_HOUR   1
#define MODE_SETTIME_MIN    2
#define MODE_SETTIME_SEC    3
#define MODE_DISP_VERSION   9
uint8_t   Mode  = MODE_NORMAL;
#define DISP_VERSION_MSEC 5000 // msec

boolean AdjustFlag = false;

RTC_DS1307  Rtc;

DateTime CurTime;

uint8_t PinSeg[] = {
  6, 7, 8, 9, 10, 11, 12, A1
};
unsigned long SegN = sizeof(PinSeg) / sizeof(uint8_t);
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

uint8_t PinDigit[] = {
  A2, A3, 5, 13, A0, 2
};
unsigned long DigitN = sizeof(PinDigit) / sizeof(uint8_t);

unsigned long Prev_millis = 0;

uint8_t dispBuffer[] = {0, 1, 2, 3, 4, 5};
//=======================================================================
boolean isLongPressed(unsigned long start_msec, unsigned long cur_msec) {
  if ( cur_msec - start_msec > BUTTON_LONGPRESSED_MSEC ) {
    return true;
  }
  return false;
}

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
    ButtonMode_PressStart = cur_msec;
    Serial.print("ButtonMode_PressStart:");
    Serial.println(ButtonMode_PressStart);
    ButtonMode_LongPressed = false;
    ButtonMode_Repeat = false;

    setMode();
  } else {
    Serial.println("PIN_BUTTON_MODE: HIGH");
    ButtonMode_PressStart = 0;
    ButtonMode_LongPressed = false;
    ButtonMode_Repeat = false;
  }

  if (digitalRead(PIN_BUTTON_COUNT) == LOW) {
    Serial.println("PIN_BUTTON_COUNT: LOW");
    ButtonCount_PressStart = cur_msec;
    Serial.print("ButtonCount_PressStart:");
    Serial.println(ButtonCount_PressStart);
    ButtonCount_LongPressed = false;
    ButtonCount_Repeat = false;

    incTime();
  } else {
    Serial.println("PIN_BUTTON_COUNT: HIGH");
    ButtonCount_PressStart = 0;
    ButtonCount_LongPressed = false;
    ButtonCount_Repeat = false;
  }
}

void setMode() {
  if ( Mode == MODE_NORMAL ) {
    // Do nothing !! Mode = MODE_SETTIME_HOUR;
  } else if ( Mode == MODE_SETTIME_HOUR ) {
    Mode = MODE_SETTIME_MIN;
    Serial.println("* MODE_SETTIME_MIN");
  } else if ( Mode == MODE_SETTIME_MIN ) {
    AdjustFlag = true;
    ButtonMode_PressStart = 0;
    ButtonMode_LongPressed = false;
    Mode = MODE_NORMAL;
    Serial.println("* MODE_SETTIME_MIN");
  } else if ( Mode == MODE_SETTIME_SEC ) {
    AdjustFlag = true;
    Mode = MODE_NORMAL;
    Serial.println("* MODE_NORMAL");
  } else {
    Mode = MODE_NORMAL;
    Serial.println("? MODE_NORMAL");
  }
}

void incTime() {
  uint8_t   hour    = CurTime.hour();
  uint8_t   minute  = CurTime.minute();
  uint8_t   second  = CurTime.second();

  if ( Mode == MODE_SETTIME_HOUR ) {
    hour = (hour + 1) % 24;
  }
  if ( Mode == MODE_SETTIME_MIN ) {
    minute = (minute + 1) % 60;
  }
  if ( Mode == MODE_SETTIME_SEC ) {
    second = 0;
  }

  CurTime = DateTime(CurTime.year(), CurTime.month(), CurTime.day(), hour, minute, second);
  //Rtc.adjust(CurTime);
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
  boolean dp = false;

  for (int i = 0; i < DigitN; i++) {
    if ( i == 1 || i ==  3 ) {
      dp = true;
    } else {
      dp = false;
    }
    if ( dispBuffer[i] == NUM_CLR ) {
      dp = false;
    }
    dispOneDigit(DigitN - i - 1, dispBuffer[i], dp);
    delay(DISP_DELAY);
  }
}

//
// check Button
// IN and OUT:
//   press_start
//   long_pressed
//   repeat
//
void checkButton(unsigned long cur_msec, uint8_t pin, unsigned long *press_start, boolean *long_pressed, boolean *repeat) {
  if ( *press_start > 0 ) {
    if ( digitalRead(pin) == LOW ) {
      if ( *long_pressed ) {
        // long pressed then check repeat
        if ( ! *repeat ) {
          if ( cur_msec - *press_start > BUTTON_REPEAT_MSEC ) {
            *repeat = true;
            Serial.println("Repeat: true");
            *press_start = cur_msec;
          }
        } // *repeat: false
      } else { // *long_pressed: false
        if ( cur_msec - *press_start > BUTTON_LONGPRESSED_MSEC ) {
          *long_pressed = true;
          Serial.println("LongPressed: true");
          *repeat = false;
          *press_start = cur_msec;
        }
      } // *long_pressed: false
    } else { // pin: HIGHT
      *press_start = 0;
      *long_pressed = false;
      *repeat = false;
    } // digitalRead(pin)
  } // *press_start > 0
}

//
// Display digits
//
void displayVFD() {
  if ( Mode == MODE_DISP_VERSION ) {
    if ( millis() % BLINK_INTERVAL > BLINK_ON_MSEC ) {
      for ( int i = 0; i < DigitN; i++ ) {
        dispBuffer[i] = NUM_CLR;
      }
    } else {
      dispBuffer[0] = VERSION[0] - '0';
      dispBuffer[1] = VERSION[1] - '0';
      dispBuffer[2] = VERSION[3] - '0';
      dispBuffer[3] = VERSION[4] - '0';
      dispBuffer[4] = VERSION[6] - '0';
      dispBuffer[5] = VERSION[7] - '0';
    }
  } else {
    dispBuffer[0] = CurTime.hour()    / 10;
    dispBuffer[1] = CurTime.hour()    % 10;
    dispBuffer[2] = CurTime.minute()  / 10;
    dispBuffer[3] = CurTime.minute()  % 10;
    dispBuffer[4] = CurTime.second()  / 10;
    dispBuffer[5] = CurTime.second()  % 10;
  }

  if ( dispBuffer[0] == 0 ) {
    dispBuffer[0] = NUM_CLR;
  }

  if ( Mode == MODE_SETTIME_HOUR ) {
    if ( digitalRead(PIN_BUTTON_COUNT) == HIGH && millis() % BLINK_INTERVAL > BLINK_ON_MSEC ) {
      dispBuffer[0] = dispBuffer[1] = NUM_CLR;
    }
    dispBuffer[4] = dispBuffer[5] = 0;
  }
  if ( Mode == MODE_SETTIME_MIN ) {
    if ( digitalRead(PIN_BUTTON_COUNT) == HIGH && millis() % BLINK_INTERVAL > BLINK_ON_MSEC ) {
      dispBuffer[2] = dispBuffer[3] = NUM_CLR;
    }
    dispBuffer[4] = dispBuffer[5] = 0;
  }

  dispAllDigit();
}
//=======================================================================
void setup() {
  Serial.begin(115200);
  Serial.print("Version: ");
  Serial.println(VERSION);
  
  Wire.begin();
  Rtc.begin();

  Serial.print("SegN = ");
  Serial.println(SegN);
  Serial.print("DigitN = ");
  Serial.println(DigitN);

  for (int i = 0; i < SegN; i++) {
    pinMode(PinSeg[i], OUTPUT);
  }
  for (int i = 0; i < DigitN; i++) {
    pinMode(PinDigit[i], OUTPUT);
  }
  pinMode(PIN_BUTTON_MODE, INPUT_PULLUP);
  pinMode(PIN_BUTTON_COUNT, INPUT_PULLUP);

  Mode = MODE_DISP_VERSION;
  Serial.println("MODE_NORMAL");
  ButtonMode_PressStart = 0;
  ButtonMode_LongPressed = false;
  ButtonMode_Repeat = false;
  ButtonCount_PressStart = 0;
  ButtonCount_LongPressed = false;
  ButtonCount_Repeat = false;

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
  }

  //
  // check long pressed or repeat
  //
  checkButton(cur_msec, PIN_BUTTON_MODE, &ButtonMode_PressStart, &ButtonMode_LongPressed, &ButtonMode_Repeat);
  checkButton(cur_msec, PIN_BUTTON_COUNT, &ButtonCount_PressStart, &ButtonCount_LongPressed, &ButtonCount_Repeat);

  //
  // process depend on current mode
  //
  if ( Mode == MODE_NORMAL ) {
    if ( ButtonMode_LongPressed ) {
      Mode = MODE_SETTIME_HOUR;
      Serial.println("* MODE_SETTIME_HOUR");
      return;
    }
    if ( AdjustFlag ) {
      CurTime = DateTime(CurTime.year(), CurTime.month(), CurTime.day(), CurTime.hour(), CurTime.minute(), 0);
      Rtc.adjust(CurTime);
      AdjustFlag = false;
    }
    CurTime = Rtc.now();

  } else if ( Mode == MODE_SETTIME_HOUR || Mode == MODE_SETTIME_MIN ) {
    if ( ButtonCount_Repeat ) {
      incTime();
      ButtonCount_PressStart = cur_msec;
      ButtonCount_Repeat = false;
    }
  }

  //
  // display
  //
  displayVFD();
} // loop()
