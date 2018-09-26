// Button.h
// (c) 2018
//
#ifndef BUTTON_H
#define BUTTON_H
#include <Arduino.h>

typedef unsigned long	btn_id_t;
typedef uint8_t		btn_e_val_t;
typedef uint8_t		btn_count_t;
typedef unsigned long	btn_msec_t;

typedef struct {
  btn_e_val_t	e_val;
  btn_msec_t	msec;
  btn_count_t	click_count;
}			btn_event_t;

typedef struct {
  boolean	val;
  btn_msec_t	msec;
}			btn_intr_t;

class IntrQ {
 public:
  static btn_intr_t	*BtnIntr;

  IntrQ();
  
  void		init(size_t n);
  void		put(boolean val, btn_msec_t msec);
  btn_intr_t	*get();
  btn_intr_t	*look();
  size_t	size() const;
  boolean	empty();
  
 private:
  volatile size_t	_qmax, _qsize, _head, _bottom;
};

class Button {
 public:
  static const unsigned long	DEBOUNCE        	=   10;
  static const unsigned long	LONG_PRESS_MSEC 	= 1000;
  static const unsigned long	REPEAT_MSEC     	=  100;
  static const unsigned long	CLICK_INTERVAL_MSEC	=  250;

  static const btn_e_val_t	EVENT_NONE		= 0x00;
  static const btn_e_val_t	EVENT_INTERRUPT		= 0x10;
  static const btn_e_val_t	EVENT_PRESSED		= 0x11;
  static const btn_e_val_t	EVENT_RELEASED		= 0x12;
  static const btn_e_val_t	EVENT_CLICKED		= 0x20;
  static const btn_e_val_t	EVENT_LONG_PRESSED 	= 0x40;
  static const btn_e_val_t	EVENT_REPEAT		= 0x80;

  static unsigned long		Num;
  static const unsigned long	NUM_MAX = 10;
  static Button			*Btn[];

  static const unsigned long	INTR_N	= 16;

  //------------------------------------------------------------
  Button();

  void		init(byte pin , String name);

  boolean	loop();

  boolean	is_enabled() const;
  void		enable();
  void		disable();

  boolean	read();
  boolean	value() const;
  boolean	prev_value() const;
  
  boolean	interrupted() const;
  void		set_interrupted(boolean flag, btn_msec_t msec);
  boolean	get_interrupted();
  btn_msec_t	interrupted_msec() const;

  btn_msec_t	press_start_msec() const;
  btn_msec_t	press_end_msec() const;
  btn_count_t	click_count() const;

  void		qput(boolean val, btn_msec_t msec);
  btn_intr_t*	qget();
  btn_intr_t*	qlook();
  size_t	qsize();
  boolean	qempty();
  
  boolean	get_event(btn_event_t *event);

  void		print();
  void		println();
  
 private:
  btn_id_t	_id;
  byte		_pin;
  String	_name;

  boolean	_is_enabled;

  boolean	_interrupted;
  boolean	_prev_value;	// HIGH/LOW
  boolean	_value;		// HIGH/LOW

  btn_msec_t	_interrupted_msec;

  btn_msec_t	_press_start_msec;
  btn_msec_t	_press_end_msec;
  btn_msec_t	_first_press_start_msec;

  btn_count_t	_click_count;
  
  boolean	_long_pressed;
  boolean	_repeat;

  IntrQ		_q;
  
  void		pciSetup(byte pin);
};
#endif
