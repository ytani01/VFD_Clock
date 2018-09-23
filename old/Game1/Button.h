// Button.h
// (c) 2018 FabLab Kannai
//
#ifndef BUTTON_H
#define BUTTON_H
#include <Arduino.h>

class Button {
  public:
    static const unsigned long LONG_PRESS_MSEC = 1000;
    static const unsigned long REPEAT_MSEC     =  100;
    static const unsigned long MULTI_MSEC      =  300;
    
    Button();
    Button(byte pin, String name);
    void    init(byte pin, String name);

    boolean get();

    String  name();
    boolean value();
    byte    count();
    boolean long_pressed();
    boolean repeat();

  private:
    String        _name;
    byte           _pin;
    boolean       _value;         // HIGH/LOW
    boolean       _prev_value;    // HIGH/LOW
    unsigned long _press_start;   // msec
    unsigned long _prev_pressed;  // msec
    byte          _count;
    boolean       _long_pressed;
    boolean       _repeat;

    void pciSetup(byte pin);
};

#endif
