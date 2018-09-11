// VFD.h
// (c) 2018 FabLab Kannai
//
#ifndef VFD_H
#define VFD_H
#include <Arduino.h>

class VFD {
  public:
    static const uint8_t DISP_DELAY = 2; // msec

    static const uint8_t VAL_BAR_HIGH = 10;
    static const uint8_t VAL_BAR_MID  = 11;
    static const uint8_t VAL_BAR_LOW  = 12;
    static const uint8_t VAL_NUL      = 0xff;
    
    VFD();
    VFD(uint8_t *pin_seg, uint8_t segN, uint8_t *pin_digit, uint8_t digitN);
    void init(uint8_t *pin_seg, uint8_t segN, uint8_t *pin_digit, uint8_t digitN);
    void clearBuf();

    uint8_t segN();
    uint8_t digitN();
    uint8_t digitI();

    void setBuf(uint8_t idx, uint8_t val);
    void setBuf(uint8_t idx, uint8_t val, boolean dp);

    void displayOne(boolean dp);
    void displayOne();
    void display();

  private:
    boolean *Val0;
    boolean Val1[];
    boolean Val2[];
    boolean Val3[];
    boolean Val4[];
    boolean Val5[];
    boolean Val6[];
    boolean Val7[];
    boolean Val8[];
    boolean Val9[];
    boolean ValBarHigh[];
    boolean ValBarMid[];
    boolean ValBarLow[];
    boolean **Val;

    uint8_t *_pin_seg;
    uint8_t _segN = 0;
//    boolean **_val;
    boolean *_dp;

    uint8_t *_pin_digit;
    uint8_t _digitN = 0;
    uint8_t _digitI = 0;

    uint8_t *_buf;
};
#endif
