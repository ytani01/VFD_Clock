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
  // bit pattern
  // 0-------  .. OK
  // 1-------  .. ERR
  // -0------  ...N/A
  // --01----  .... DISP
  // --10----  .... SET
  // ----01--  ...... DATE
  // ----10--  ...... TIME
  // ------00  ........ ALL
  // ------01  ........ YEAR/HOUR
  // ------10  ........ MONTH/MINUTE
  // ------11  ........ DAY/SECOND
  static const mode_t MODEMASK_OK_ERR		= 0x80;
  static const mode_t MODEMASK_DISP_SET		= 0x30;
  static const mode_t MODEMASK_DATE_TIME	= 0x09;
  static const mode_t MODE_DISP			= 0x10;
  static const mode_t MODE_DISP_DATE		= 0x14;
  static const mode_t MODE_DISP_TIME		= 0x18;
  static const mode_t MODE_SET			= 0x20;
  static const mode_t MODE_SET_DATE		= 0x24;
  static const mode_t MODE_SET_DATE_YEAR	= 0x25;
  static const mode_t MODE_SET_DATE_MONTH	= 0x26;
  static const mode_t MODE_SET_DATE_DAY		= 0x27;
  static const mode_t MODE_SET_TIME		= 0x28;
  static const mode_t MODE_SET_TIME_HOUR	= 0x29;
  static const mode_t MODE_SET_TIME_MINUTE	= 0x2A;
  static const mode_t MODE_SET_TIME_SECOND	= 0x2B;
  static const mode_t MODE_ERR			= 0x80;


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

