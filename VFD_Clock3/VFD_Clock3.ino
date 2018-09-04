/*
   $Id:$
*/
#include "RTClib.h"
// #include <FlexiTimer2.h>
#include <Wire.h>

#if 1
#define DEGUB
#endif

#define DISP_DELAY  2 // msec

#define PIN_SCL   A5
#define PIN_SDA   A4

#define PIN_BUTTON_MODE		3
#define PIN_BUTTON_COUNT	4

#define BUTTON_DEBOUNCE   100  // msec
#define BUTTON_LONGPRESSED_COUNT    60

unsigned long ButtonMode_PressCount = 0; // 0: false
unsigned long ButtonCount_PressCount = 0; // 0: false
boolean ButtonMode_LongPressed = false;
boolean ButtonCount_LongPressed = false;

boolean AdjustFlag = false;

RTC_DS1307  Rtc;

DateTime CurTime;

#define MODE_NORMAL         0
#define MODE_TIMESET_HOUR   1
#define MODE_TIMESET_MIN    2
#define MODE_TIMESET_SEC    3
uint8_t   Mode  = MODE_NORMAL;

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


void pciSetup(byte pin) {
  *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));   // enable pin
  PCIFR  |= bit (digitalPinToPCICRbit(pin));                    // clear any outstanding interrupt
  PCICR  |= bit (digitalPinToPCICRbit(pin));                    // enable interrupt for the group
}

ISR (PCINT2_vect) {
  if (millis() - Prev_millis < BUTTON_DEBOUNCE) {
    return;
  }

  if (digitalRead(PIN_BUTTON_MODE) == LOW) {
    ButtonMode_PressCount = 1;
    Serial.println("ButtonMode_PressCount: 1");
    ButtonMode_LongPressed = false;
    Serial.println("ButtonMode_LongPressed: false");

    setMode();
  }
  if (digitalRead(PIN_BUTTON_COUNT) == LOW) {
    ButtonCount_PressCount = 1;
    Serial.println("ButtonCount_PressCount: 1");
    ButtonCount_LongPressed = false;
    Serial.println("ButtonCount_LongPressed: false");

    setTime();
  }

  Prev_millis = millis();
}

void setMode() {
  if ( Mode == MODE_NORMAL ) {
    // Do nothing !! Mode = MODE_TIMESET_HOUR;
  } else if ( Mode == MODE_TIMESET_HOUR ) {
    Mode = MODE_TIMESET_MIN;
  } else if ( Mode == MODE_TIMESET_MIN ) {
    AdjustFlag = true;
    Mode = MODE_NORMAL;
  } else if ( Mode == MODE_TIMESET_SEC ) {
    AdjustFlag = true;
    Mode = MODE_NORMAL;
  } else {
    Mode = MODE_NORMAL;
  }
}

void setTime() {
  // do nothing !
}

/*
   Write one digit
*/
void writeDigit(uint8_t digit, uint8_t num, boolean dp) {
  for (int i = 0; i < DigitN; i++) {
    digitalWrite(PinDigit[i], LOW);
  }

  if ( num > 9 ) {
    num = NUM_CLR;
  }
  for (int i = 0; i < SegN - 1; i++) {
    digitalWrite(PinSeg[i], Num[num][i]);
  }

  digitalWrite(PinSeg[SegN - 1], dp);

  digitalWrite(PinDigit[digit], HIGH);
}

/*
   Display all digits
*/
void displayVFD() {
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
    writeDigit(DigitN - i - 1, dispBuffer[i], dp);
    delay(DISP_DELAY);
  }
}
//=======================================================================
void setup() {
  Serial.begin(115200);
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

  Mode = MODE_NORMAL;
  ButtonMode_PressCount = 0;
  ButtonMode_LongPressed = false;
  ButtonCount_PressCount = 0;
  ButtonCount_LongPressed = false;

  pciSetup(PIN_BUTTON_MODE);
  pciSetup(PIN_BUTTON_COUNT);
  EICRA |= bit (ISC11);
  sei();

  for (int i = 0; i < DigitN; i++) {
    writeDigit(i, 8, true);
    delay(500);
  }
} // setup()
//=======================================================================
void loop() {
  uint8_t val[] = {1, 2, 3, 4, 5, 6};

  // Mode Button
  if ( ButtonMode_PressCount > 0 ) {
    if ( digitalRead(PIN_BUTTON_MODE) == LOW ) {
      ButtonMode_PressCount++;
      if ( ! ButtonMode_LongPressed ) {
        if (ButtonMode_PressCount % 10 == 0 ) {
          Serial.print("ButtonMode_PressCount: ");
          Serial.println(ButtonMode_PressCount);
        }
        if ( ButtonMode_PressCount > BUTTON_LONGPRESSED_COUNT ) {
          ButtonMode_LongPressed = true;
          Serial.println("ButtonMode_LongPressed: true");
        }
      }
    } else { // PIN_BUTTON_MODE: HIGH
      ButtonMode_PressCount = 0;
      ButtonMode_LongPressed = false;
    }
  }
  
  // Count Button
  if ( ButtonCount_PressCount > 0 ) {
    if ( digitalRead(PIN_BUTTON_COUNT) == LOW ) {
      ButtonCount_PressCount++;
      if ( ! ButtonCount_LongPressed ) {
        if ( ButtonCount_PressCount % 10 == 0 ) {
          Serial.print("ButtonCount_PressCount: ");
          Serial.println(ButtonCount_PressCount);
        }
        if ( ButtonCount_PressCount > BUTTON_LONGPRESSED_COUNT ) {
          ButtonCount_LongPressed = true;
          Serial.println("ButtonCount_LongPressed: true");
        }
      }
    } else { // PIN_BUTTON_COUNT: HIGH
      ButtonCount_PressCount = 0;
      ButtonCount_LongPressed = false;
    }
  }

  if ( Mode == MODE_NORMAL ) {
    if ( ButtonMode_LongPressed ) {
      Mode = MODE_TIMESET_HOUR;
      return;
    }

    if ( AdjustFlag ) {
      CurTime = DateTime(CurTime.year(), CurTime.month(), CurTime.day(), CurTime.hour(), CurTime.minute(), 0);
      Rtc.adjust(CurTime);
      AdjustFlag = false;
    }

    CurTime = Rtc.now();
  }

  if ( ButtonCount_PressCount == 2 || ( ButtonCount_LongPressed && (ButtonCount_PressCount % 20 == 0)) ) {
    uint16_t  year    = CurTime.year();
    uint8_t   month   = CurTime.month();
    uint8_t   day     = CurTime.day();
    uint8_t   hour    = CurTime.hour();
    uint8_t   minute  = CurTime.minute();
    uint8_t   second  = CurTime.second();

    if ( Mode == MODE_TIMESET_HOUR ) {
      hour = (hour + 1) % 24;
    }

    if ( Mode == MODE_TIMESET_MIN ) {
      minute = (minute + 1) % 60;
    }

    if ( Mode == MODE_TIMESET_SEC ) {
      second = 0;
    }

    CurTime = DateTime(year, month, day, hour, minute, second);
    Rtc.adjust(CurTime);
  }

  dispBuffer[0] = CurTime.hour()    / 10;
  dispBuffer[1] = CurTime.hour()    % 10;
  dispBuffer[2] = CurTime.minute()  / 10;
  dispBuffer[3] = CurTime.minute()  % 10;
  dispBuffer[4] = CurTime.second()  / 10;
  dispBuffer[5] = CurTime.second()  % 10;

  if ( Mode == MODE_TIMESET_HOUR ) {
    dispBuffer[2] = dispBuffer[3] = dispBuffer[4] = dispBuffer[5] = NUM_CLR;
  }
  if ( Mode == MODE_TIMESET_MIN ) {
    dispBuffer[0] = dispBuffer[1] = dispBuffer[4] = dispBuffer[5] = NUM_CLR;
  }
  if ( Mode == MODE_TIMESET_SEC ) {
    dispBuffer[0] = dispBuffer[1] = dispBuffer[2] = dispBuffer[3] = NUM_CLR;
  }

  displayVFD();
  //delay(1);
} // loop()
