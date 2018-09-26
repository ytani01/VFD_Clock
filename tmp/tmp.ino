//
#include "Button.h"

#define		PIN_BUTTON_MODE	3
#define		PIN_BUTTON_SET	4
#define		BUTTON_N	2

Button *Btn;

//====================================================================
void setup() {
  Serial.begin(115200);
  while ( !Serial ) {}	// for Leonardo

  Serial.println("setup()");

  Btn = new Button[BUTTON_N];
  Btn[0].init(PIN_BUTTON_MODE, "[MODE]");
  Btn[1].init(PIN_BUTTON_SET , "[SET]");

  sei();
}
//--------------------------------------------------------------------
void loop() {
  unsigned long	cur_msec = millis();
  btn_event_t	btn_event;

  for (int i = 0; i < BUTTON_N; i++) {
    while ( Btn[i].get_event(&btn_event) ) {
      if ( btn_event.e_val & Button::EVENT_INTERRUPT ) {
	continue;
      }
      
      Serial.println(String(cur_msec) + ":Btn[" + String(i) + "]:" +
		     "0x" + String(btn_event.e_val,16) +
		     "," +
		     String(btn_event.msec) +
		     "," +
		     String(btn_event.click_count) +
		     ":" +
		     String(Btn[i].press_start_msec()) +
		     "," +
		     String(Btn[i].press_end_msec()) +
		     " " +
		     String(Btn[i].click_count())
		     );
    } // while
  } // for

  delay(2);
}
