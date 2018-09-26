// Button.cpp
// (c) 2018
//
#include "Button.h"

static unsigned long	Button::Num = 0;
static Button		*Button::Btn[Button::NUM_MAX];
static btn_intr_t	*IntrQ::BtnIntr;

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
      btn->qput(val, cur_msec);
    }
  }
  //  Serial.println();
}
//================================================================
IntrQ::IntrQ()
{
}
//----------------------------------------------------------------
void IntrQ::init(size_t n)
{
  _qmax = n;
  BtnIntr = new btn_intr_t[_qmax];
  _head = 0;
  _bottom = 0;
  _qsize = 0;
}
//----------------------------------------------------------------
void IntrQ::put(boolean val, btn_msec_t msec)
{
  if ( _qsize == _qmax ) {
    return;
  }
  BtnIntr[_bottom].val	= val;
  BtnIntr[_bottom].msec	= msec;

  _bottom = ( _bottom + 1 ) % _qmax;
  _qsize++;
}
//----------------------------------------------------------------
btn_intr_t *IntrQ::get()
{
  btn_intr_t	*ret;
  
  if ( _qsize == 0 ) {
    return (btn_intr_t *)0;
  }
  ret = &BtnIntr[_head];

  _head = ( _head + 1 ) % _qmax;
  _qsize--;
  return ret;
}
//----------------------------------------------------------------
btn_intr_t *IntrQ::look()
{
  if ( _qsize == 0 ) {
    return (btn_intr_t *)0;
  }
  return &BtnIntr[_head];
}
//----------------------------------------------------------------
size_t IntrQ::size() const
{
  return _qsize;
}
//----------------------------------------------------------------
boolean IntrQ::empty()
{
  if ( _qsize == 0 ) {
    return true;
  }
  return false;
}
//================================================================
Button::Button()
{
}
//----------------------------------------------------------------
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

  _first_press_start_msec = _press_start_msec = _press_end_msec = 0;

  _click_count	= 0;
  
  _long_pressed = false;
  _repeat	= false;

  _q.init(INTR_N);
  
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
btn_count_t Button::click_count() const
{
  return _click_count;
}
//----------------------------------------------------------------
void Button::qput(boolean val, btn_msec_t msec)
{
  _q.put(val, msec);
}
//----------------------------------------------------------------
btn_intr_t *Button::qget()
{
  return _q.get();
}
//----------------------------------------------------------------
btn_intr_t *Button::qlook()
{
  return _q.look();
}
//----------------------------------------------------------------
size_t Button::qsize()
{
  return _q.size();
}
//----------------------------------------------------------------
boolean Button::qempty()
{
  return _q.empty();
}
//----------------------------------------------------------------
boolean Button::get_event(btn_event_t *event) {
  btn_msec_t	cur_msec = millis();
  boolean	val	= read();
  btn_intr_t	*intr;
  
  event->e_val = EVENT_NONE;
  event->msec = cur_msec;
  event->click_count = 0;

  if ( ! qempty() ) {
    intr = qlook();
    if ( _click_count > 0 ) {
      if ( intr->msec - _press_start_msec > CLICK_INTERVAL_MSEC ) {
	event->e_val = EVENT_CLICKED;
	event->msec = intr->msec;
	event->click_count = _click_count;
	
	_click_count = 0;
	_first_press_start_msec = 0;
	_press_start_msec = 0;
	_press_end_msec = 0;
	return true;
      }
    }
    
    intr = qget();
    event->msec = intr->msec;
    _long_pressed = false;
    if ( intr->val == LOW ) {
      event->e_val = EVENT_PRESSED;

      if ( event->msec - _press_start_msec > CLICK_INTERVAL_MSEC ) {
	_click_count = 0;
	_first_press_start_msec = event->msec;
      }
      _press_start_msec	= event->msec;
      _press_end_msec	= event->msec;

      _click_count++;
    } else {
      event->e_val = EVENT_RELEASED;
      
      _press_end_msec = event->msec;
    }
    return true;
  }

  if ( _click_count > 0 ) {
    if ( cur_msec - _press_start_msec > CLICK_INTERVAL_MSEC ) {
      event->e_val = EVENT_CLICKED;
      event->msec = cur_msec;
      event->click_count = _click_count;

      _click_count = 0;
      _first_press_start_msec = 0;
    }
  }
  
  if ( event->e_val != EVENT_NONE ) {
    return true;
  } else {
    return false;
  }
}
//----------------------------------------------------------------
void Button::print()
{
  Serial.print("[" + String(_id) + ":" + String(_pin) + ":" + _name + "]");
}
//----------------------------------------------------------------
void Button::println()
{
  print();
  Serial.println();
}
//================================================================
void Button::pciSetup(byte pin)
{
  *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));	// enable pin
  PCIFR  |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
  PCICR  |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group
}
//================================================================
