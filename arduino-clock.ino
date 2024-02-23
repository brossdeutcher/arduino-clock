#include <RTClib.h>
#include <TM1637Display.h>
#include "cantina.h"

// Red LED
#define led 8

// buzzer
#define buzzer 9

// Rotary Encoder
#define rotarySW 2
#define rotaryDT 3
#define rotaryCLK 4
int rotaryCounter = 0;
int currentStateRotaryCLK;
int lastStateRotaryCLK;
int currentStateRotaryDT;
int currentStateRotarySW;
int lastStateRotarySW;

// Real-Time Clock DS1307
RTC_DS1307 rtc;
DateTime now;

// Seven-Segment Display
#define tm1637DIO 5
#define tm1637CLK 6
TM1637Display sevSegDisplay = TM1637Display(tm1637CLK, tm1637DIO);
const uint8_t blank[] = { 0x00, 0x00, 0x00, 0x00 };
const uint8_t all_on[] = { 0xff, 0xff, 0xff, 0xff };
const uint8_t alrm[] = {
  SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G,
  SEG_D | SEG_E | SEG_F,
  SEG_A | SEG_E | SEG_F,
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G
};

// misc
int curModeInt = 0;
char curModeStr = "curTime";
int displayMode[] = {"curTime", "setHr", "setMin", "setAlrmHr", "setAlrmMin"};
DateTime alarmTime = DateTime(62*60);  // boot alarm time


void setup() {

  // Red LED
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);
  delay(100);
  digitalWrite(led, LOW);
  
  // Seven Segment Display
  sevSegDisplay.clear();
  sevSegDisplay.setBrightness(5);
  sevSegDisplay.clear();
  sevSegDisplay.setSegments(alrm);
  delay(500);
  
  // Rotary Encoder
  pinMode(rotarySW, INPUT_PULLUP);
  // attachInterrupt(digitalPinToInterrupt(rotarySW), changeMode, FALLING);
  pinMode(rotaryDT, INPUT);
  pinMode(rotaryCLK, INPUT);
  Serial.begin(9600);
  lastStateRotaryCLK = digitalRead(rotaryCLK);
  lastStateRotarySW = digitalRead(rotarySW);
  
  // RTC
  rtc.begin();
  rtc.adjust(DateTime(2023, 9, 7, 4, 20, 58)); // boot time

}

void loop() {

  now = rtc.now();

  sevSegHandler();
  rotaryEncoderHandler();

  if (now.hour() == alarmTime.hour() && now.minute() == alarmTime.minute() && now.second() < 10) {
    playCantina(led, buzzer);
  }
}

void rotaryEncoderHandler() {
  currentStateRotarySW = digitalRead(rotarySW);
  if (currentStateRotarySW == 1 && lastStateRotarySW == 0) {
    changeMode();
  }
  lastStateRotarySW = currentStateRotarySW;

  currentStateRotaryCLK = digitalRead(rotaryCLK);
  currentStateRotaryDT = digitalRead(rotaryDT);
  if (currentStateRotaryCLK != lastStateRotaryCLK && currentStateRotaryCLK == 1) {
    // timeChange();
    if (currentStateRotaryDT != currentStateRotaryCLK) {
      if (curModeInt == 1) {
        rtc.adjust(rtc.now() - TimeSpan(0,1,0,0));
        Serial.print("clock hr --");
      }
      else if (curModeInt == 2) {
        rtc.adjust(rtc.now() - TimeSpan(0,0,1,0));
        Serial.print("clock min --");
      }
      else if (curModeInt == 3) {
        alarmTime = alarmTime - TimeSpan(0,1,0,0);
        Serial.println("alarm hr --");
      }
      else if (curModeInt == 4) {
        alarmTime = alarmTime - TimeSpan(0,0,1,0);
        Serial.println("alarm min --");
      }
    } else {
      if (curModeInt == 1) {
        rtc.adjust(rtc.now() + TimeSpan(0,1,0,0));
        Serial.print("clock hr ++");
      }
      else if (curModeInt == 2) {
        rtc.adjust(rtc.now() + TimeSpan(0,0,1,0));
        Serial.print("clock min ++");
      }
      else if (curModeInt == 3) {
        alarmTime = alarmTime + TimeSpan(0,1,0,0);
        Serial.println("alarm hr ++");
      }
      else if (curModeInt == 4) {
        alarmTime = alarmTime + TimeSpan(0,0,1,0);
        Serial.println("alarm min ++");
      }
    }
  }
  lastStateRotaryCLK = currentStateRotaryCLK;
}

void timeChange() {
  if (curModeInt == 0) {
    return;
  }
 
  if (digitalRead(currentStateRotaryDT != currentStateRotaryCLK)) {
    Serial.println("++");
  } else {
    Serial.println("--");
  }
}

void changeMode() {
  curModeInt = (curModeInt + 1) % 5;
  sevSegDisplay.clear();

  Serial.print("current mode?: ");
  Serial.println(curModeInt);
}

void sevSegHandler() {
  if (curModeInt == 0) {
    sevSegDisplay.showNumberDecEx(now.minute(), 0, true, 2, 2);
    sevSegDisplay.showNumberDecEx(now.hour(), 0b01000000, true, 2, 0);
  } else if (curModeInt == 1) {
    sevSegDisplay.showNumberDecEx(now.hour(), 0b01000000, true, 2, 0);
  } else if (curModeInt == 2) {
    // TODO: get minute curTime display to show colon :
    sevSegDisplay.showNumberDecEx(now.minute(), 0b01000000, true, 2, 2);
  } else if (curModeInt == 3) {
    sevSegDisplay.showNumberDecEx(alarmTime.hour(), 0, true, 2, 0);
  } else if (curModeInt == 4) {
    sevSegDisplay.showNumberDecEx(alarmTime.minute(), 0, true, 2, 2);
  }
}
