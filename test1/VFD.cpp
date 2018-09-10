// VFD.cpp
// (c) 2018 FabLab Kannai
//
#include "VFD.h"

// Constractor
VFD::VFD(uint8_t *pin_seg, uint8_t *pin_digit, boolean **val) {
  _pin_seg = pin_seg;
  _pin_digit = pin_digit;
  _val = val;
}

// Public methods

// Private methods
