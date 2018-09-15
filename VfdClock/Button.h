// Button.h
// (c) 2018 FabLab Kannai
//
#ifndef BUTTON_H
#define BUTTON_H
#include <Arduino.h>

typedef unsigned long id_t;
typedef uint8_t button_event_t;

class Button {
 public:
  static const unsigned long DEBOUNCE        	=   10;
  static const unsigned long LONG_PRESS_MSEC 	= 1000;
  static const unsigned long REPEAT_MSEC     	=  100;
  static const unsigned long MULTI_MSEC 	=  500;

  static const button_event_t EVENT_NONE		= 0x00;
  static const button_event_t EVENT_INTRRUPT		= 0x10;
  static const button_event_t EVENT_PESSED		= 0x11;
  static const button_event_t EVENT_RELEASED		= 0x12;
  static const button_event_t EVENT_LONG_PRESSED 	= 0x02;
  static const button_event_t EVENT_REPEAT		= 0x03;

  static unsigned long	Num;
  static Button		**BTN;

  static void 		interruptHandler();
  static void 		loopHandler();

  Button();
  Button(byte pin, String name);

  void    	init(byte pin, String name);

  boolean	get();

  void		enable();
  void		disable();
  boolean	isEnabled();

  String  	name();
  boolean 	value();
  byte    	count();
  boolean	long_pressed();
  boolean	repeat();
  uint8_t	multi_count();

  void    	print();

 private:
  id_t		_id;
  String        _name;
  byte           _pin;
  boolean       _value;      // HIGH/LOW
  boolean       _prev_value; // HIGH/LOW
  unsigned long _first_press_start; // msec
  unsigned long _press_start;       // msec
  byte          _count;
  boolean       _long_pressed;
  boolean       _repeat;
  uint8_t	_multi_count;

  boolean       _is_enabled = true;

  void pciSetup(byte pin);
};

#endif
