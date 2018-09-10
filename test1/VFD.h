// VFD.h
// (c) 2018 FabLab Kannai
//
#include <Arduino.h>

class VFD {
  public:
    VFD(uint8_t *pin_seg, uint8_t *pin_digit, boolean **val);

  private:
    uint8_t *_pin_seg;
    uint8_t _segN = 0;
    boolean **_val;
    
    uint8_t *_pin_digit;
    uint8_t _digitN = 0;
    uint8_t _digitI = 0;
};
