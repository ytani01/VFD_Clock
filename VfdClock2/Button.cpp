// Button.cpp
// (c) 2018 FabLab Kannai
//
#include "Button.h"

static unsigned long	Button::Num = 0;
static Button		**Button::BTN = new Button*[10];

// static methods
// XXX
static void
Button::interruptHandler()
{
  unsigned long		cur_msec = millis();
  static uint8_t	prev_pin;
  static unsigned long	prev_msec = 0;

  if ( cur_msec - prev_msec < DEBOUNCE ) {
    return;
  }
  prev_msec = cur_msec;

  for (int i=0; i < Num; i++) {
    //Serial.println(String(i) + ":" + BTN[i]->name());
  }
}

// Constractor
Button::Button()
{
  BTN[Num] = this;
  _id = Num;
  Num++;

}

// public methods
void Button::init(byte pin, String name)
{
  _pin          = pin;
  _name         = name;

  _value        = HIGH;
  _prev_value   = HIGH;
  _press_start  = 0;
  _first_press_start = 0;
  _count        = 0;
  _long_pressed = false;
  _repeat       = false;

  _is_enabled   = true;

  pinMode(_pin, INPUT_PULLUP);

  pciSetup(_pin);
}

boolean Button::get()
{
  unsigned long cur_msec = millis();
  boolean 	ret = false;
  
  if ( ! _is_enabled ) {
    return false;
  }

  // is Enabled
  
  _prev_value = _value;
  _value = digitalRead(_pin);

  _multi_count = 0;
  if ( _count > 0 ){
    if ( cur_msec - _first_press_start > MULTI_MSEC ) {
      _multi_count = _count;
      _count = 0;
      ret = true;
    }
  }

  if ( _value == HIGH ) {
    // Released button then refresh some flags and do nothing any more
    _press_start = 0;
    _long_pressed = false;
    _repeat = false;

    if ( _value != _prev_value ) {
      return true;
    }
    return ret;
  }

  // LOW
  if ( _value != _prev_value ) {
    
    // Pushed now !
    _press_start = cur_msec;
    _count++;
    if ( _count == 1 ) {
      _first_press_start = cur_msec;
    }
    return true;
  }

  // continueing pressed
  if ( ! _long_pressed ) {
    if ( cur_msec - _press_start > LONG_PRESS_MSEC ) {
      _long_pressed = true;
      _press_start = cur_msec;
      return true;
    } else {
      return ret;
    }
  }

  if ( cur_msec - _press_start > REPEAT_MSEC ) {
    _repeat = true;
    _press_start = cur_msec;
    return true;
  }

  return ret;
}

void Button::enable()
{
  _is_enabled = true;
}
void Button::disable()
{
  _is_enabled = false;
}

boolean Button::isEnabled()
{
  return _is_enabled;
}

String Button::name()
{
  return _name;
}
boolean Button::value()
{
  return _value;
}
byte Button::count()
{
  return _count;
}
boolean Button::long_pressed()
{
  return _long_pressed;
}
boolean Button::repeat()
{
  return _repeat;
}
uint8_t Button::multi_count()
{
  return _multi_count;
}

void Button::print()
{
  Serial.print(_name);
  if ( _value ) {
    Serial.print(":-");
  } else {
    Serial.print(":*");
  }
  Serial.print(" " + String(_count));
  Serial.print(" " + String(_multi_count));
  if ( _long_pressed ) {
    Serial.print(" L");
  } else {
    Serial.print(" -");
  }
  if ( _repeat ) {
    Serial.print(" R");
  } else {
    Serial.print(" -");
  }
  Serial.println();
}

// private methods
void Button::pciSetup(byte pin)
{
  *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));   // enable pin
  PCIFR  |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
  PCICR  |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group
}
