// VFD Clock Game2
// (c) 2018 FabLab Kannai
//
String VersionStr = "01.00.00";
#define DISP_VERSION_MSEC  2000  // msec

#include <Wire.h>
#include "RTClib.h"
#include "Button.h"
#include "VFD.h"
#include "Game2.h"

#define BLINK_INTERVAL           500 // msec
#define BLINK_ON_MSEC            350 // msec
#define BUTTON_DEBOUNCE           10 // msec

#define BULLET_INTERVAL          250 // msec
#define ENEMY_INTERVAL          3000 // msec

#define PIN_BUTTON_MODE   3
#define PIN_BUTTON_SET    4

uint8_t PinSeg[]     = { 6, 7, 8, 9, 10, 11, 12, A1 };

uint8_t PinDigit[]   = { 2, A0, 13, 5, A3, A2 };

#define MODE_DISP_VERSION         0x00
#define MODE_PLAY                 0x10
#define MODE_END                  0x20
uint8_t Mode = MODE_DISP_VERSION;

unsigned long CurMsec  = 0;
unsigned long PrevMsec = 0;

VFD    Vfd;
Button ButtonMode, ButtonSet;
Player P1;
Enemy  E1;
unsigned long Score = 0;

RTC_DS1307 Rtc;
//=========================================================
ISR (PCINT2_vect) {
  unsigned long cur_msec = millis();
  static unsigned long prev_msec = 0;

  if ( cur_msec - prev_msec < BUTTON_DEBOUNCE ) {
    return;
  }
  //  Serial.println("ISR()");
  prev_msec = cur_msec;

  if (ButtonMode.get()) {
    if ( ButtonMode.value() == LOW ) {
      P1.up();
      //      Serial.println("P1: " + String(P1.val()));
    }
  }
  if (ButtonSet.get()) {
    if (ButtonSet.value() == LOW ) {
      P1.shoot(BULLET_INTERVAL);
      //      Serial.println("P1: shoot: " + String(P1.bullet().val()));
    }
  }
}
//---------------------------------------------------------
boolean blinkOff() {
  if ( CurMsec % BLINK_INTERVAL > BLINK_ON_MSEC ) {
    return true;
  }
  return false;
}

void displayVersion() {
  if ( CurMsec > DISP_VERSION_MSEC ) {
    Mode = MODE_PLAY;
    return;
  }
  Vfd.setBuf(0, VersionStr[0] - '0', false);
  Vfd.setBuf(1, VersionStr[1] - '0', true);
  Vfd.setBuf(2, VersionStr[3] - '0', false);
  Vfd.setBuf(3, VersionStr[4] - '0', true);
  Vfd.setBuf(4, VersionStr[6] - '0', false);
  Vfd.setBuf(5, VersionStr[7] - '0', false);
}

void displayGame() {
  Vfd.clearBuf();
}

void displayScore() {
  Vfd.clearBuf();
  if ( blinkOff() ) {
    return;
  }

  for (int i = 0; i < Vfd.digitN(); i++) {
    Vfd.setBuf(i, Score / int(pow(10, 5 - i)) % 10, false);
  }
}

void displayVFD() {
  if ( Mode == MODE_DISP_VERSION ) {
    displayVersion();
  }

  if ( Mode == MODE_PLAY ) {
    displayGame();
  }

  if ( Mode == MODE_END ) {
    displayScore();
  }

  Vfd.displayOne();
}
//=========================================================
void setup() {
  Serial.begin(115200);

  while (!Serial) {} // for Leonardo

  Serial.println("Version: " + VersionStr);

  Rtc.begin();
  unsigned long sec = Rtc.now().second();
  Serial.println("sec = " + String(sec));
  randomSeed(sec);

  Vfd.init(PinSeg, sizeof(PinSeg) / sizeof(PinSeg[0]), PinDigit, sizeof(PinDigit) / sizeof(PinDigit[0]));
  ButtonMode.init(PIN_BUTTON_MODE, "[MODE]");
  ButtonSet.init(PIN_BUTTON_SET,   "[SET]");

  P1.init(0);
  Score = 0;

  Mode = MODE_DISP_VERSION;
  Serial.println("Vfd.digitN() = " + String(Vfd.digitN()));
  sei();
}

void loop() {
  CurMsec = millis();

  if (ButtonMode.get()) {
    Serial.println(ButtonMode.name() + ": " + String(ButtonMode.count()) + ", " + String(ButtonMode.long_pressed()) + ", " + String(ButtonMode.repeat()));
    if ( ButtonMode.long_pressed() || ButtonMode.repeat() ) {
    }
  }
  if (ButtonSet.get()) {
    if ( ButtonSet.long_pressed() || ButtonSet.repeat() ) {
    }
  }

  // move
  if ( P1.bullet_move() ) {

  }
  if ( E1.generate() ) {
    if ( E1.x() == 0 ) {
      Mode = MODE_END;
    }
  }

  // check and update()
  if ( P1.bullet_val() != Bullet::VAL_NULL && E1.size() > 0 ) {
    if ( P1.bullet_x() == E1.x() ) {
      if ( P1.bullet_val() == E1.val(0) ) {
        // hit !
        E1.hit();
        Score++;
        Serial.println("Score=" + String(Score));
        if ( E1.interval() > 500 ) {
          E1.set_interval(E1.interval() - 100);
          Serial.println("E1.interval() = " + String(E1.interval()));
        }
      }
      P1.bullet_delete();
    }
  }

  displayVFD();
}
