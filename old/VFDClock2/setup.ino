void swUpdate() {
  int val = analogRead(PIN_BUTTON_MODE);
  if (val < 300) {  // SW1
    if (pmillis[SW1_ON] == 0 || pmillis[SW1_OFF] != 0) {
      pmillis[SW1_ON]  = millis();
      pmillis[SW1_OFF] = 0;
    }
  } else if (pmillis[SW1_ON] != 0) {
    if (pmillis[SW1_OFF] == 0)
      pmillis[SW1_OFF] = millis();
  }

  val = analogRead(PIN_BUTTON_COUNT);
  if (val < 300) {  // SW2
    if (pmillis[SW2_ON] == 0 || pmillis[SW2_OFF] != 0) {
      pmillis[SW2_ON]  = millis();
      pmillis[SW2_OFF] = 0;
    }
  } else if (pmillis[SW2_ON] != 0) {
    if (pmillis[SW2_OFF] == 0)
      pmillis[SW2_OFF] = millis();
  }
}

void setTime() {
  switch (mode) {
    case MODE_TIMESET_HOUR:
      if (++dHH > 23) dHH = 0;
      break;

    case MODE_TIMESET_MIN:
      if (++dMM > 59) dMM = 0;
      break;
  }
}

