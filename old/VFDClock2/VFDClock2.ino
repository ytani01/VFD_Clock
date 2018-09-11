#include "RTClib.h"
#include <Wire.h>

RTC_DS1307 rtc;

#define PIN_SER           2
#define PIN_SRCLK         3
#define PIN_SRCLR         4
#define PIN_RCLK          5

#define PIN_SEG_A         6
#define PIN_SEG_B         7
#define PIN_SEG_C         8
#define PIN_SEG_D         9
#define PIN_SEG_E         10
#define PIN_SEG_F         11
#define PIN_SEG_G         12

#define PIN_SEG_H         A0
#define PIN_SEG_DP        A1
#define PIN_SEG_MINUS     A2
#define PIN_SDA           A4
#define PIN_SCL           A5
#define PIN_BUTTON_COUNT  A6  // SW2
#define PIN_BUTTON_MODE   A7  // SW1

#define INTERVAL           11
#define NUM_DIGITS          6
#define BUTTON_COUNT_SHORT  2
#define BUTTON_COUNT_LONG   8

uint8_t dHH = 23, dMM = 59, dSS = 59;
uint8_t digits[NUM_DIGITS];

enum { MODE_NORMAL = 100, MODE_TIMESET_HOUR, MODE_TIMESET_MIN, MODE_TIMESET_EXIT };
size_t mode = MODE_NORMAL;

enum { LOOP_1000, BLINKS, MODE_SET, SW1_ON, SW1_OFF, SW2_ON, SW2_OFF, STATE_END };
uint32_t pmillis[STATE_END];

uint16_t holdD[] = {  800,  800,  800,  800,  800,  800 };  // 6 digit

void setup() {
  pinMode(PIN_SEG_A, OUTPUT);
  pinMode(PIN_SEG_B, OUTPUT);
  pinMode(PIN_SEG_C, OUTPUT);
  pinMode(PIN_SEG_D, OUTPUT);
  pinMode(PIN_SEG_E, OUTPUT);
  pinMode(PIN_SEG_F, OUTPUT);
  pinMode(PIN_SEG_G, OUTPUT);
  pinMode(PIN_SEG_H, OUTPUT);
  pinMode(PIN_SEG_MINUS, OUTPUT);
  pinMode(PIN_SEG_DP, OUTPUT);
  pinMode(PIN_BUTTON_MODE,  INPUT_PULLUP);
  pinMode(PIN_BUTTON_COUNT, INPUT_PULLUP);
  pinMode(PIN_SER, OUTPUT);
  pinMode(PIN_SRCLK, OUTPUT);
  pinMode(PIN_SRCLR, OUTPUT);
  pinMode(PIN_RCLK, OUTPUT);

  digitalWrite(PIN_SRCLR, HIGH);

  Serial.begin(115200);
  Wire.begin();
  rtc.begin();

  if (!rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(__DATE__, __TIME__));
  }
  Serial.println("Start");
}

#define PASTms(i) (pmillis[i] ? (millis() - pmillis[i]) : 0)

void loop() {
  DateTime preDT, nowDT;
  uint8_t dots = 0, blinks = 0, updateVFD = 0;
  uint16_t repStep = 0, repTimes[3] = { 80, 800, 400 };

  while (1) {
    swUpdate();

    // SW2(count-up)
    if (mode != MODE_NORMAL) {
      if (PASTms(MODE_SET) > 60000) { // Timeout
        mode = MODE_NORMAL;
        pmillis[LOOP_1000] = 0;
        
      } else {
        if (PASTms(SW2_ON) > repTimes[repStep]) {
          setTime();
          if (pmillis[SW2_OFF] != 0) repStep = pmillis[SW2_ON] = pmillis[SW2_OFF] = 0;
          if (pmillis[SW2_ON] != 0) { // Keep pushing ?
            if (PASTms(SW2_ON) > repTimes[repStep]) {
              pmillis[SW2_ON] += repTimes[repStep];
              if (repStep < 2) repStep++;
            }
          }
          pmillis[MODE_SET] = millis();
          updateVFD = 1;
        } else {
          if (pmillis[SW2_OFF] != 0) repStep = pmillis[SW2_ON] = pmillis[SW2_OFF] = 0;
        }
      }
    }

    // SW1(mode change)
    if ((PASTms(SW1_ON) > 80) && PASTms(SW1_OFF) > 80) {
      pmillis[SW1_ON] = pmillis[SW1_OFF] = 0;
      if (++mode >= MODE_TIMESET_EXIT) {  // mode : NORMAL -> SET_HOUR -> SET_MIN -> SET_EXIT(NORMAL)
        DateTime tmpDT = rtc.now();
        DateTime newDT = DateTime(tmpDT.year(), tmpDT.month(), tmpDT.day(), dHH, dMM, 0);
        rtc.adjust(newDT);
        mode = MODE_NORMAL;
      } else {
        pmillis[MODE_SET] = millis();
      }
      pmillis[LOOP_1000] = 0;
    }

    if ((pmillis[LOOP_1000] == 0) || (PASTms(LOOP_1000) >= 1000)) {
      if (mode == MODE_NORMAL) {
        nowDT = rtc.now();
        dHH = nowDT.hour();
        dMM = nowDT.minute();
        dSS = nowDT.second();
        preDT = nowDT;
      }
      updateVFD = 1;
    }

    if (updateVFD) {
      if (mode != MODE_NORMAL) dSS = 0;
      digits[5] = dHH / 10; digits[4] = dHH % 10;
      digits[3] = dMM / 10; digits[2] = dMM % 10;
      digits[1] = dSS / 10; digits[0] = dSS % 10;
      pmillis[LOOP_1000] = millis();
      pmillis[BLINKS] = 0;
      updateVFD = 0;
    }

    if (mode == MODE_NORMAL) {
      if ((pmillis[BLINKS] == 0) || (PASTms(BLINKS) >= 500)) {
        dots = (pmillis[BLINKS] == 0) ? B010100 : 0;
        pmillis[BLINKS] = millis();
      }
      blinks = B111111;
    } else {
      if ((pmillis[BLINKS] == 0) || (PASTms(BLINKS) >= 180)) {
        if (pmillis[BLINKS] != 0)
          if (blinks == B111111)
            blinks = ((mode == MODE_TIMESET_HOUR) ? B001111 : B110011);
          else
            blinks = B111111;
        pmillis[BLINKS] = millis();
      }
      dots = 0;
    }
    writeVFD(blinks, dots);
  }
}

