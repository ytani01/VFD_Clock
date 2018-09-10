// Button.cpp
// (c) 2018 FabLab Kannai
//
#include "Button.h"

// Constractor
Button::Button() {
}

// public methods
void Button::init(byte pin, String name) {
  _pin          = pin;
  _name         = name;
  _value        = HIGH;
  _prev_value   = HIGH;
  _press_start  = 0;
  _prev_pressed = 0;
  _count        = 0;
  _long_pressed = false;
  _repeat       = false;

  pinMode(_pin, INPUT_PULLUP);

  pciSetup(_pin);
}

boolean Button::get() {
  unsigned long cur_msec = millis();

  _prev_value = _value;
  _value = digitalRead(_pin);

  if ( _value != _prev_value ) {
    if ( _value == LOW ) {
      _press_start = cur_msec;
      if ( cur_msec - _prev_pressed > MULTI_MSEC ) {
        _count = 0;
        _prev_pressed = cur_msec;
      }
      _count++;
    } else { // HIGH
      _press_start = 0;
      _long_pressed = false;
      _repeat = false;
    }
    Serial.println(_name + ": " + String(_value) + ": " + String(_count) + ", " + String(_prev_pressed) + ", " + String(_press_start));
    return true;
  }

  // unchanged
  if ( _value == HIGH ) {
    return false;
  }

  // continueing pressed
  if ( ! _long_pressed ) {
    if ( cur_msec - _press_start > LONG_PRESS_MSEC ) {
      _long_pressed = true;
      _press_start = cur_msec;
      Serial.println(_name + ": long pressed");
      return true;
    } else {
      return false;
    }
  }

  if ( cur_msec - _press_start > REPEAT_MSEC ) {
    _repeat = true;
    _press_start = cur_msec;
    Serial.println(_name + ": repeat");
    return true;
  }

  return false;
}

String Button::name() {
  return _name;
}
boolean Button::value() {
  return _value;
}
byte Button::count() {
  return _count;
}
boolean Button::long_pressed() {
  return _long_pressed;
}
boolean Button::repeat() {
  return _repeat;
}

// private methods
void Button::pciSetup(byte pin) {
  *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));   // enable pin
  PCIFR  |= bit (digitalPinToPCICRbit(pin));                    // clear any outstanding interrupt
  PCICR  |= bit (digitalPinToPCICRbit(pin));                    // enable interrupt for the group
}
