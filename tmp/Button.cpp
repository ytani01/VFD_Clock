// Button.cpp
// (c) 2018
//
#include "Button.h"

static unsigned long	Button::Num = 0;
static Button		*Button::Btn[Button::NUM_MAX];

//================================================================
// Interrupt Handler
//================================================================
ISR (PCINT2_vect)
{
  static uint8_t       prev_pin;
  unsigned long        cur_msec = millis();

  //  Serial.print(String(cur_msec) + ":ISR ");

  for (int i=0; i < Button::Num; i++) {
    Button 	*btn	= Button::Btn[i];
    boolean	val	= btn->read();
    
    if ( val != btn->prev_value() ) {
      if ( cur_msec - btn->interrupted_msec() < Button::DEBOUNCE ) {
	continue;
      }
      btn->set_interrupted(true, cur_msec);
      if ( val == LOW ) {
	btn->start_press(cur_msec);
      } else { // HIGH
	btn->end_press(cur_msec);
      }
      //      Serial.print("[" + String(i) + "]:"
      //	   + String(btn->prev_value())
      //	   + "->"
      //	   + String(val) );
    }
  }
  //  Serial.println();
}
//================================================================
Button::Button()
{
}
//================================================================
void Button::init(byte pin, String name)
{
  _id		= Num++;
  Btn[_id]	= this;
  _pin		= pin;
  _name		= name;

  enable();

  set_interrupted(false, 0);

  _prev_value	= HIGH;
  _value	= HIGH;

  start_press(0);
  end_press(0);
  
  pinMode(_pin, INPUT_PULLUP);
  pciSetup(_pin);
}
//----------------------------------------------------------------
boolean Button::loop()
{
}
//----------------------------------------------------------------
boolean Button::is_enabled() const
{
  return _is_enabled;
}
//----------------------------------------------------------------
void Button::enable()
{
  _is_enabled = true;
}
//----------------------------------------------------------------
void Button::disable()
{
  _is_enabled = false;
}
//----------------------------------------------------------------
boolean Button::read()
{
  _prev_value = _value;
  _value = digitalRead(_pin);
  return _value;
}
//----------------------------------------------------------------
boolean Button::value() const
{
  return _value;
}
//----------------------------------------------------------------
boolean Button::prev_value() const
{
  return _prev_value;
}
//----------------------------------------------------------------
boolean Button::interrupted() const
{
  return _interrupted;
}
//----------------------------------------------------------------
void Button::set_interrupted(boolean flag, btn_msec_t msec)
{
  _interrupted = flag;
  _interrupted_msec = msec;
}
//----------------------------------------------------------------
btn_msec_t Button::interrupted_msec() const
{
  return _interrupted_msec;
}
//----------------------------------------------------------------
boolean Button::get_interrupted()
{
  if ( interrupted() ) {
    _interrupted = false;
    return true;
  }
  return false;
}
//----------------------------------------------------------------
btn_msec_t Button::press_start_msec() const
{
  return _press_start_msec;
}
//----------------------------------------------------------------
btn_msec_t Button::press_end_msec() const
{
  return _press_end_msec;
}
//----------------------------------------------------------------
void Button::start_press(btn_msec_t msec)
{
  _press_start_msec = msec;
  _press_end_msec = 0;
}
//----------------------------------------------------------------
void Button::end_press(btn_msec_t msec)
{
  _press_end_msec = msec;
}
//----------------------------------------------------------------
btn_event_t Button::get_event() {
  btn_msec_t	cur_msec	= millis();
  btn_event_t	event		= EVENT_NONE;
  boolean	val		= read();

  if ( get_interrupted() ) {
    if ( val == LOW ) {
      event = EVENT_PRESSED;
    } else {
      event = EVENT_RELEASED;
    }
    return event;
  }
  
  return event;
}
//================================================================
void Button::pciSetup(byte pin)
{
  *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));	// enable pin
  PCIFR  |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
  PCICR  |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group
}
