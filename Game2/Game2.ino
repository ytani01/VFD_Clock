// VFD Clock Game2
// (c) 2018 FaLab Kannai
//
String VersionStr = "05.00.00";

#define STARTUP_MSEC  3000  // msec

#include <Wire.h>
#include "RTClib.h"
#include "Button.h"
#include "VFD.h"
#include "Clock.h"

#define PIN_BUTTON_MODE   3
#define PIN_BUTTON_SET    4
uint8_t PinSeg[]     = { 6, 7, 8, 9, 10, 11, 12, A1 };
uint8_t PinDigit[]   = { 2, A0, 13, 5, A3, A2 };

#define	MODE_STARTUP	0x00
#define MODE_CLOCK	0x01
uint8_t Mode = MODE_STARTUP;

unsigned long CurMsec  = 0;
unsigned long PrevMsec = 0;

VFD    Vfd;
Button ButtonMode, ButtonSet;
Clock  Clock1;

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
      switch ( Clock1.mode() ) {
      case Clock::MODE_DISP_TIME:
	Clock1.set_mode(Clock::MODE_DISP_DATE);
	break;
      case Clock::MODE_DISP_DATE:
	Clock1.set_mode(Clock::MODE_DISP_TIME);
	break;
      case Clock::MODE_SET_DATE_YEAR:
	Clock1.set_mode(Clock::MODE_SET_DATE_MONTH);
	break;
      case Clock::MODE_SET_DATE_MONTH:
	Clock1.set_mode(Clock::MODE_SET_DATE_DAY);
	break;
      case Clock::MODE_SET_DATE_DAY:
	Clock1.set_mode(Clock::MODE_SET_TIME_HOUR);
	break;
      case Clock::MODE_SET_TIME_HOUR:
	Clock1.set_mode(Clock::MODE_SET_TIME_MINUTE);
	break;
      case Clock::MODE_SET_TIME_MINUTE:
	Clock1.set_mode(Clock::MODE_SET_TIME_SECOND);
	break;
      case Clock::MODE_SET_TIME_SECOND:
	Clock1.set_adjust_flag(true);
	Clock1.set_mode(Clock::MODE_DISP_TIME);
	break;
      } // switch
    }
  }
  if (ButtonSet.get()) {
    ButtonSet.print();
    if (ButtonSet.value() == LOW ) {
      switch ( Clock1.mode() ) {
      case Clock::MODE_SET_DATE_YEAR:
	Clock1.countUpYear();
	break;
      case Clock::MODE_SET_DATE_MONTH:
	Clock1.countUpMonth();
	break;
      case Clock::MODE_SET_DATE_DAY:
	Clock1.countUpDay();
	break;
      case Clock::MODE_SET_TIME_HOUR:
	Clock1.countUpHour();
	break;
      case Clock::MODE_SET_TIME_MINUTE:
	Clock1.countUpMinute();
	break;
      case Clock::MODE_SET_TIME_SECOND:
	Clock1.countUpSecond();
	break;
      } // switch ( Clock1.mode() )
    }
  }
}
//---------------------------------------------------------
void displayVersion() {
  if ( CurMsec > STARTUP_MSEC ) {
    Mode = MODE_CLOCK;
    return;
  }
  Vfd.set(0, VersionStr[0] - '0', false, true);
  Vfd.set(1, VersionStr[1] - '0', true,  true);
  Vfd.set(2, VersionStr[3] - '0', false, true);
  Vfd.set(3, VersionStr[4] - '0', true,  true);
  Vfd.set(4, VersionStr[6] - '0', false, true);
  Vfd.set(5, VersionStr[7] - '0', false, true);

  Vfd.display();
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
  Clock1.init(&Rtc, &Vfd);

  sei();
}

void loop() {
  CurMsec = millis();
  static boolean blink_sw = true;

  // Button Check
  if (ButtonMode.get()) {
    ButtonMode.print();
    if ( ButtonMode.long_pressed() || ButtonMode.repeat() ) {
      switch ( Clock1.mode() ) {
      case Clock:: MODE_DISP_TIME:
      case Clock:: MODE_DISP_DATE:
	Clock1.set_mode(Clock::MODE_SET_DATE_YEAR);
      } // switch ( Clock1.mode() )
    }
  }
  if (ButtonSet.get()) {
    ButtonSet.print();
    if ( ButtonSet.long_pressed() || ButtonSet.repeat() ) {
      blink_sw = false;
      switch ( Clock1.mode() ) {
      case Clock::MODE_SET_DATE_YEAR:
	Clock1.countUpYear();
	break;
      case Clock::MODE_SET_DATE_MONTH:
	Clock1.countUpMonth();
	break;
      case Clock::MODE_SET_DATE_DAY:
	Clock1.countUpDay();
	break;
      case Clock::MODE_SET_TIME_HOUR:
	Clock1.countUpHour();
	break;
      case Clock::MODE_SET_TIME_MINUTE:
	Clock1.countUpMinute();
	break;
      case Clock::MODE_SET_TIME_SECOND:
	Clock1.countUpSecond();
	break;
      } // switch
    }
  } else if ( ! blink_sw ) {
    if ( ! ButtonSet.long_pressed() ) {
      blink_sw = true;
    }
  }

  if ( Clock1.adjust_flag() ) {
    Clock1.adjust();
    Clock1.set_adjust_flag(false);
  }

  // Display
  switch ( Mode ) {
  case MODE_STARTUP:
    displayVersion();
    break;	
  case MODE_CLOCK:
    Clock1.display(blink_sw);
    break;
  } // switch ( Mode )
}
