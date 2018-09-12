// VFD Clock Game2
// (c) 2018 FaLab Kannai
//
String VersionStr = "01.00.00";

#define DISP_VERSION_MSEC  3000  // msec

#include <Wire.h>
#include "RTClib.h"
#include "Button.h"
#include "VFD.h"
#include "Game2.h"

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
  unsigned long        cur_msec = millis();
  static uint8_t       prev_pin;
  static unsigned long prev_msec = 0;

  if ( cur_msec - prev_msec < Button::DEBOUNCE ) {
    return;
  }
  prev_msec = cur_msec;

  if (ButtonMode.get()) {
    ButtonMode.print();
    if ( ButtonMode.value() == LOW ) {
      P1.up();
    }
  }
  if (ButtonSet.get()) {
    ButtonSet.print();
    if (ButtonSet.value() == LOW ) {
      P1.shoot(BULLET_INTERVAL);
    }
  }
}
//---------------------------------------------------------
void displayVersion() {
  if ( CurMsec > DISP_VERSION_MSEC ) {
    Mode = MODE_PLAY;
    Serial.println("MODE_PLAY");
    return;
  }
  Vfd.set(0, VersionStr[0] - '0', false, true);
  Vfd.set(1, VersionStr[1] - '0', true,  true);
  Vfd.set(2, VersionStr[3] - '0', false, false);
  Vfd.set(3, VersionStr[4] - '0', true,  false);
  Vfd.set(4, VersionStr[6] - '0', false, false);
  Vfd.set(5, VersionStr[7] - '0', false, false);
}

void displayGame() {
  Vfd.clear();
  if ( CurMsec > 5000 ) {
    Mode = MODE_END;
  }
}

void displayScore() {
  boolean blink = false;
  uint8_t v[] = {0xa, 0xb, VFD::VAL_NULL, VFD::VAL_NULL, 0xe, 0xf};

  if ( CurMsec % 4000 > 2000 ) {
    blink = true;
  }
  uint8_t v1 = (CurMsec / 10000) % 0x10;
  uint8_t v2 = (CurMsec /  1000) % 0x10;
  
  Vfd.setValue(v);
  Vfd.setValue(2, v1);
  Vfd.setValue(3, v2);
  Vfd.setDp(false);
  Vfd.setDp(1, true);
  Vfd.setDp(3, true);
  Vfd.setDp(4, true);
  Vfd.setDp(5, true);
  Vfd.setBlink(blink);
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

  size_t digit_n = sizeof(PinDigit) / sizeof(PinDigit[0]);
  Vfd.init(PinSeg, PinDigit, digit_n);
  ButtonMode.init(PIN_BUTTON_MODE, "[MODE]");
  ButtonSet.init(PIN_BUTTON_SET,   "[SET]");

  P1.init(0);
  Score = 0;

  Mode = MODE_DISP_VERSION;
  sei();
}

void loop() {
  CurMsec = millis();

  // Button Check
  if (ButtonMode.get()) {
    ButtonMode.print();
    if ( ButtonMode.long_pressed() || ButtonMode.repeat() ) {
    }
  }
  if (ButtonSet.get()) {
    ButtonSet.print();
    if ( ButtonSet.long_pressed() || ButtonSet.repeat() ) {
    }
  }

  // Move
  if ( P1.bullet_move() ) {

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

  // Display
  switch ( Mode ) {
  case MODE_DISP_VERSION:
    displayVersion();
    break;
  case MODE_PLAY:
    displayGame();
    break;
  case MODE_END:
    displayScore();
    break;
  default:
    break;
  }
  Vfd.display();
}
