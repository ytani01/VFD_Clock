// VFD.cpp
// (c) 2018 FabLab Kannai
//
#include "VFD.h"

// Constractor
VFD::VFD() {
}
VFD::VFD(uint8_t *pin_seg, uint8_t segN, uint8_t *pin_digit, uint8_t digitN) {
  init(pin_seg, segN, pin_digit, digitN);
}

// Public methods
void VFD::init(uint8_t *pin_seg, uint8_t segN, uint8_t *pin_digit, uint8_t digitN) {
  Val0    = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, LOW };
  Val1    = {LOW , HIGH, HIGH, LOW , LOW , LOW , LOW };
  Val2    = {HIGH, HIGH, LOW , HIGH, HIGH, LOW , HIGH};
  Val3    = {HIGH, HIGH, HIGH, HIGH, LOW , LOW , HIGH};
  Val4    = {LOW , HIGH, HIGH, LOW , LOW , HIGH, HIGH};
  Val5    = {HIGH, LOW , HIGH, HIGH, LOW , HIGH, HIGH};
  Val6    = {HIGH, LOW , HIGH, HIGH, HIGH, HIGH, HIGH};
  Val7    = {HIGH, HIGH, HIGH, LOW , LOW , LOW , LOW };
  Val8    = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};
  Val9    = {HIGH, HIGH, HIGH, HIGH, LOW , HIGH, HIGH};
  ValBarHigh = {HIGH, LOW , LOW , LOW , LOW , LOW , LOW };
  ValBarMid  = {LOW , LOW , LOW , LOW , LOW , LOW , HIGH};
  ValBarLow  = {LOW , LOW , LOW , HIGH, LOW , LOW , LOW };
  Val    = {Val0, Val1, Val2, Val3, Val4, Val5, Val6, Val7, Val8, Val9, ValBarHigh, ValBarMid, ValBarLow};

  _pin_seg = pin_seg;
  _segN = segN;
  for (int i = 0; i < _segN; i++) {
    pinMode(_pin_seg[i], OUTPUT);
    digitalWrite(_pin_seg[i], LOW);
  }

  _pin_digit = pin_digit;
  _digitN = digitN;
  _digitI = 0;
  _dp = new boolean[_digitN];
  for (int i = 0; i < _digitN; i++) {
    _dp[i] = false;
    pinMode(_pin_digit[i], OUTPUT);
    digitalWrite(_pin_digit[i], LOW);
  }

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
void VFD::setBuf(uint8_t idx, uint8_t val, boolean dp) {
  _buf[idx] = val;
  _dp[idx] = dp;
}

void VFD::displayOne(boolean dp) {
  if ( _buf[_digitI] != VAL_NUL ) {
    for (int i = 0; i < _segN; i++) {
      digitalWrite(_pin_seg[i], Val[_buf[_digitI]][i]);
    }
    digitalWrite(_pin_seg[_segN - 1], dp);
    digitalWrite(_pin_digit[_digitI], HIGH);
  }
  delay(DISP_DELAY);
  digitalWrite(_pin_digit[_digitI], LOW);
  _digitI = ( _digitI + 1 ) % _digitN;
}
void VFD::displayOne() {
  if ( _buf[_digitI] != VAL_NUL ) {
    for (int i = 0; i < _segN; i++) {
      digitalWrite(_pin_seg[i], Val[_buf[_digitI]][i]);
    }
    digitalWrite(_pin_seg[_segN - 1], _dp[_digitI]);
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
