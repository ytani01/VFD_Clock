// VFD.cpp
// (c) 2018 FabLab Kannai
//
#include "VFD.h"

// Constractor
VFD::VFD() {
}
VFD::VFD(uint8_t *pin_seg, uint8_t segN, uint8_t *pin_digit, uint8_t digitN, boolean **val) {
  init(pin_seg, segN, pin_digit, digitN, val);
}

// Public methods
void VFD::init(uint8_t *pin_seg, uint8_t segN, uint8_t *pin_digit, uint8_t digitN, boolean **val) {
  _pin_seg = pin_seg;
  _segN = segN;
  for (int i = 0; i < _segN; i++) {
    pinMode(_pin_seg[i], OUTPUT);
    digitalWrite(_pin_seg[i], LOW);
  }

  _pin_digit = pin_digit;
  _digitN = digitN;
  _digitI = 0;
  for (int i = 0; i < _digitN; i++) {
    pinMode(_pin_digit[i], OUTPUT);
    digitalWrite(_pin_digit[i], LOW);
  }

  _val = val;

  _buf = new uint8_t[_digitN];
  clearBuf();
}

void VFD::clearBuf() {
  for (int i = 0; i < _digitN; i++) {
    _buf[i] = VAL_NUL;
  }
}

uint8_t VFD::segN() {
  return _segN;
}

uint8_t VFD::digitN() {
  return _digitN;
}
uint8_t VFD::digitI() {
  return _digitI;
}

void VFD::setBuf(uint8_t idx, uint8_t val) {
  //Serial.println(String(idx) + ", " + String(val));
  _buf[idx] = val;
}

void VFD::displayOne(boolean dp) {
  if ( _buf[_digitI] != VAL_NUL ) {
    for (int i = 0; i < _segN; i++) {
      digitalWrite(_pin_seg[i], _val[_buf[_digitI]][i]);
    }
    digitalWrite(_pin_seg[_segN - 1], dp);
    digitalWrite(_pin_digit[_digitI], HIGH);
  }
  delay(DISP_DELAY);
  digitalWrite(_pin_digit[_digitI], LOW);
  _digitI = ( _digitI + 1 ) % _digitN;
}

void VFD::display() {
  displayOne(false);
}

// Private methods
