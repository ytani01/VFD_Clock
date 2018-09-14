// Game1.h
// (c) 2018 FabLab Kannai
//
#ifndef CLOCK_H
#define CLOCK_H
#include <Arduino.h>
#include <Wire.h>
#include "RTClib.h"
#include "VFD.h"

typedef uint8_t mode_t;

class Clock {
 public:
  static const mode_t MODE_DISP_DATE		= 0x10;
  static const mode_t MODE_DISP_TIME		= 0x20;
  static const mode_t MODE_SET_DATE		= 0x30;
  static const mode_t MODE_SET_DATE_YEAR	= 0x31;
  static const mode_t MODE_SET_DATE_MONTH	= 0x32;
  static const mode_t MODE_SET_DATE_DAY		= 0x33;
  static const mode_t MODE_SET_TIME		= 0x40;
  static const mode_t MODE_SET_TIME_HOUR	= 0x41;
  static const mode_t MODE_SET_TIME_MINUTE	= 0x42;
  static const mode_t MODE_SET_TIME_SECOND	= 0x43;
  static const mode_t MODE_ERR			= 0xf0;


  Clock();
  
  void		init(RTC_DS1307 *rtc, VFD *vfd);

  mode_t	mode();
  void		set_mode(mode_t mode);
  
  DateTime	cur_dt();
  String	dateStr();
  
  boolean	adjust_flag();
  void		set_adjust_flag(boolean flag);

  boolean	update();
  void		adjust();
  boolean	adjustIfNecessary();

  void		countUpYear();
  void		countUpMonth();
  void		countUpDay();
  void		countUpHour();
  void		countUpMinute();
  void		countUpSecond();

  void		setVfd(unsigned long num1, unsigned long num2, unsigned long num3);
  void		setVfdDate();
  void		setVfdTime();
  
  void		displayDate();
  void		displayTime();
  void		displaySetDate();
  void		displaySetTime();
  void		display(boolean blink_sw);

 private:
  RTC_DS1307	*_rtc;
  VFD		*_vfd;
  
  mode_t	_mode		= MODE_DISP_TIME;
  boolean	_adjust_flag	= false;

  DateTime	_cur_dt;
  unsigned long _msec_offset	= 0; // msec
};

#endif

