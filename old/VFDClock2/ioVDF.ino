void clearSeg() {
  digitalWrite(PIN_SEG_A, LOW);
  digitalWrite(PIN_SEG_B, LOW);
  digitalWrite(PIN_SEG_C, LOW);
  digitalWrite(PIN_SEG_D, LOW);
  digitalWrite(PIN_SEG_E, LOW);
  digitalWrite(PIN_SEG_F, LOW);
  digitalWrite(PIN_SEG_G, LOW);
  digitalWrite(PIN_SEG_DP, LOW);
  digitalWrite(PIN_SEG_MINUS, LOW);
}

void decodeSeg(int num, uint8_t dp, uint8_t minus) {
  if (dp)    digitalWrite(PIN_SEG_DP,    HIGH);
  if (minus) digitalWrite(PIN_SEG_MINUS, HIGH);

  switch (num) {
    case 0:
      digitalWrite(PIN_SEG_A, HIGH);
      digitalWrite(PIN_SEG_B, HIGH);
      digitalWrite(PIN_SEG_C, HIGH);
      digitalWrite(PIN_SEG_D, HIGH);
      digitalWrite(PIN_SEG_E, HIGH);
      digitalWrite(PIN_SEG_F, HIGH);
      break;
    case 1:
      digitalWrite(PIN_SEG_B, HIGH);
      digitalWrite(PIN_SEG_C, HIGH);
      break;
    case 2:
      digitalWrite(PIN_SEG_A, HIGH);
      digitalWrite(PIN_SEG_B, HIGH);
      digitalWrite(PIN_SEG_G, HIGH);
      digitalWrite(PIN_SEG_E, HIGH);
      digitalWrite(PIN_SEG_D, HIGH);
      break;
    case 3:
      digitalWrite(PIN_SEG_A, HIGH);
      digitalWrite(PIN_SEG_B, HIGH);
      digitalWrite(PIN_SEG_G, HIGH);
      digitalWrite(PIN_SEG_C, HIGH);
      digitalWrite(PIN_SEG_D, HIGH);
      break;
    case 4:
      digitalWrite(PIN_SEG_F, HIGH);
      digitalWrite(PIN_SEG_G, HIGH);
      digitalWrite(PIN_SEG_B, HIGH);
      digitalWrite(PIN_SEG_C, HIGH);
      break;
    case 5:
      digitalWrite(PIN_SEG_A, HIGH);
      digitalWrite(PIN_SEG_F, HIGH);
      digitalWrite(PIN_SEG_G, HIGH);
      digitalWrite(PIN_SEG_C, HIGH);
      digitalWrite(PIN_SEG_D, HIGH);
      break;
    case 6:
      digitalWrite(PIN_SEG_A, HIGH);
      digitalWrite(PIN_SEG_F, HIGH);
      digitalWrite(PIN_SEG_E, HIGH);
      digitalWrite(PIN_SEG_D, HIGH);
      digitalWrite(PIN_SEG_C, HIGH);
      digitalWrite(PIN_SEG_G, HIGH);
      break;
    case 7:
      digitalWrite(PIN_SEG_F, HIGH);
      digitalWrite(PIN_SEG_A, HIGH);
      digitalWrite(PIN_SEG_B, HIGH);
      digitalWrite(PIN_SEG_C, HIGH);
      break;
    case 8:
      digitalWrite(PIN_SEG_A, HIGH);
      digitalWrite(PIN_SEG_B, HIGH);
      digitalWrite(PIN_SEG_C, HIGH);
      digitalWrite(PIN_SEG_D, HIGH);
      digitalWrite(PIN_SEG_E, HIGH);
      digitalWrite(PIN_SEG_F, HIGH);
      digitalWrite(PIN_SEG_G, HIGH);
      break;
    case 9:
      digitalWrite(PIN_SEG_A, HIGH);
      digitalWrite(PIN_SEG_F, HIGH);
      digitalWrite(PIN_SEG_G, HIGH);
      digitalWrite(PIN_SEG_B, HIGH);
      digitalWrite(PIN_SEG_C, HIGH);
      digitalWrite(PIN_SEG_D, HIGH);
      break;
    default:
      clearSeg(); // Turn off digit if num > 10
      break;
  }
}

void writeVFD(uint8_t blinks,  uint8_t dots) {
  //digitalWrite(13, 1);
  for (int i = 0, m = 0x01; i < NUM_DIGITS; i++, m<<=1) {
    digitalWrite(PIN_RCLK, LOW);
    shiftOut(PIN_SER, PIN_SRCLK, MSBFIRST, m);
    digitalWrite(PIN_RCLK, HIGH);
    if (blinks & m) decodeSeg(digits[i], (dots & m), 0);
    delay(2);
    delayMicroseconds(holdD[i]);
    clearSeg();
  }
  //digitalWrite(13,0);
}

