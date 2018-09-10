// VFD.h
// (c) 2018 FabLab Kannai
//
#ifndef VFD_H
#define VFD_H
#include <Arduino.h>

class VFD {
  public:
    static const uint8_t DISP_DELAY = 1; // msec
    static const uint8_t VAL_NUL    = 0xff;

    VFD();
    VFD(uint8_t *pin_seg, uint8_t segN, uint8_t *pin_digit, uint8_t digitN, boolean **val);
    void init(uint8_t *pin_seg, uint8_t segN, uint8_t *pin_digit, uint8_t digitN, boolean **val);
    void clearBuf();

    uint8_t segN();
    uint8_t digitN();
    uint8_t digitI();
    
    void setBuf(uint8_t idx, uint8_t val);
    
    void displayOne(boolean dp);
    void display();

  private:
    uint8_t *_pin_seg;
    uint8_t _segN = 0;
    boolean **_val;

    uint8_t *_pin_digit;
    uint8_t _digitN = 0;
    uint8_t _digitI = 0;

    uint8_t *_buf;
};
#endif
