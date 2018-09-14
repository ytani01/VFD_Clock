// VFD Clock
// (c) 2018 FaLab Kannai
//
String VersionStr = "05.00.01";

#define STARTUP_MSEC    3000 // msec
#define DISP_DATE_MSEC  5000 // msec

#include <Wire.h>
#include "RTClib.h"
#include "Button.h"
#include "VFD.h"
#include "Clock.h"

#define PIN_BUTTON_MODE	3
#define PIN_BUTTON_SET	4
uint8_t PinSeg[]     	= { 6, 7, 8, 9, 10, 11, 12, A1 };
uint8_t PinDigit[]   	= { 2, A0, 13, 5, A3, A2 };

#define	MODE_STARTUP	0x00
#define MODE_CLOCK	0x01
uint8_t Mode = MODE_STARTUP;

RTC_DS1307	Rtc;
VFD		Vfd;
Clock		Clock1;
Button		*Btn;
#define 	BUTTON_N 2

unsigned long	CurMsec  = 0;
unsigned long	PrevMsec = 0;
unsigned long	StartUpStart = 0;
unsigned long	DateStart = 0;
boolean 	BlinkEnable = true;

//=========================================================
// [MODE] interrupt
void button0IntrHandler(unsigned long cur_msec)
{
  if ( Btn[0].value() == HIGH ) {
    return;
  }

  // LOW
  if ( Mode == MODE_STARTUP ) {
    Mode = MODE_CLOCK;
    return;
  }
  
  // Mode == MODE_CLOCK
  switch ( Clock1.mode() ) {
  case Clock::MODE_DISP_TIME:
    Clock1.set_mode(Clock::MODE_DISP_DATE);
    DateStart = cur_msec;
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
  } // switch ( Clock1.mode() )
}
//---------------------------------------------------------
// [SET] intrrupt
void button1IntrHandler(unsigned long cur_msec)
{
  if ( Btn[1].value() == HIGH ) {
    return;
  }

  // LOW
  if ( Mode == MODE_STARTUP ) {
    Mode = MODE_CLOCK;
    return;
  }

  // Mode == MODE_CLOCK
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
//---------------------------------------------------------
// button interrupt
void buttonIntrHandler(unsigned long cur_msec)
{
  for (int btn_num = 0; btn_num < BUTTON_N; btn_num++) {
    if ( Btn[btn_num].get() ) {
      Btn[btn_num].print();

      switch ( btn_num ) {
      case 0:
	button0IntrHandler(cur_msec);
	break;
      case 1:
	button1IntrHandler(cur_msec);
	break;
      default:
	break;
      } // switch
    }
  } // for
}
//---------------------------------------------------------
ISR (PCINT2_vect)
{
  unsigned long        cur_msec = millis();
  static uint8_t       prev_pin;
  static unsigned long prev_msec = 0;

  if ( cur_msec - prev_msec < Button::DEBOUNCE ) {
    return;
  }
  prev_msec = cur_msec;

  buttonIntrHandler(cur_msec);
}
//---------------------------------------------------------
// [MODE] event in loop()
void button0LoopHandler(unsigned long cur_msec)
{
  if ( Mode == MODE_STARTUP ) {
    return;
  }
  
  // MODE_CLOCK
  if ( Btn[0].long_pressed() || Btn[0].repeat() ) {
    switch ( Clock1.mode() ) {
    case Clock:: MODE_DISP_TIME:
    case Clock:: MODE_DISP_DATE:
      Clock1.set_mode(Clock::MODE_SET_DATE_YEAR);
    } // switch ( Clock1.mode() )
  }
}
//---------------------------------------------------------
// [SET] event in loop()
void button1LoopHandler(unsigned long cur_msec)
{
  if ( Mode == MODE_STARTUP ) {
    return;
  }

  // MODE_CLOCK
  if ( Btn[1].multi_count() >= 2 ) { // double click
    Mode = MODE_STARTUP;
    StartUpStart = cur_msec;
    return;
  }
  
  if ( Btn[1].long_pressed() || Btn[1].repeat() ) {
    BlinkEnable = false;
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
  } else {
    if ( ! BlinkEnable ) {
      BlinkEnable = true;
    }
  }
}
//---------------------------------------------------------
void buttonLoopHandler(unsigned long cur_msec)
{
  for (int btn_num = 0; btn_num < BUTTON_N; btn_num++) {
    if ( Btn[btn_num].get() ) {
      Btn[btn_num].print();

      switch ( btn_num ) {
      case 0:
	button0LoopHandler(cur_msec);
	break;
      case 1:
	button1LoopHandler(cur_msec);
	break;
      default:
	break;
      } // switch
    } // if
  } // for
}
//---------------------------------------------------------
void displayVersion() {
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
  Btn = new Button[BUTTON_N];
  Btn[0].init(PIN_BUTTON_MODE, "[MODE]");
  Btn[1].init(PIN_BUTTON_SET,   "[SET]");
  Clock1.init(&Rtc, &Vfd);

  sei();
}
//=========================================================
void loop() {
  CurMsec = millis();

  // Button Handler
  buttonLoopHandler(CurMsec);

  // Adjust clock
  if ( Clock1.adjustIfNecessary() ) {
    Serial.println(Clock1.dateStr());
  }

  // Timeout etc.
  switch ( Mode ) {
  case MODE_STARTUP:
    if ( CurMsec - StartUpStart > STARTUP_MSEC ) {
      Mode = MODE_CLOCK;
    }
  case MODE_CLOCK:
    if ( Clock1.mode() == Clock::MODE_DISP_DATE ) {
      if ( CurMsec - DateStart > DISP_DATE_MSEC ) {
	Clock1.set_mode(Clock::MODE_DISP_TIME);
      }
    }
  default:
    break;
  }

  // Display
  switch ( Mode ) {
  case MODE_STARTUP:
    displayVersion();
    break;	
  case MODE_CLOCK:
    Clock1.display(BlinkEnable);
    break;
  } // switch ( Mode )
}
