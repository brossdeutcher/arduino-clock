#include <RTClib.h>
#include <TM1637Display.h>
#include "cantina.h"

// Red LED
#define led 3

// buzzer
#define buzzer 4

// Rotary Encoder
#define rotarySW 8
#define rotaryDT 9
#define rotaryCLK 10
int rotaryCounter = 0;
int currentStateRotaryCLK;
int lastStateRotaryCLK;
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
  
  // Seven Segment Display
  sevSegDisplay.clear();
  sevSegDisplay.setBrightness(5);
  sevSegDisplay.clear();
  sevSegDisplay.setSegments(alrm);
  delay(1000);
  
  // Rotary Encoder
  pinMode(rotarySW, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(rotarySW), setAlarm, FALLING);
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
  curModeStr = displayMode[curModeInt];

  sevSegHandler();
  rotaryEncoderHandler();

  if (now.hour() == alarmTime.hour() && now.minute() == alarmTime.minute() && now.second() < 10) {
    playCantina(led, buzzer);
  }
}

void rotaryEncoderHandler() {
  currentStateRotarySW = digitalRead(rotarySW);
  if (currentStateRotarySW == 1 && lastStateRotarySW == 0) {
    curModeInt = (curModeInt + 1) % 5;
    sevSegDisplay.clear();
    Serial.print("rtc time: ");
    Serial.print(rtc.now().hour());
    Serial.print(":");
    Serial.println(rtc.now().minute());
    Serial.print("current mode?: ");
    Serial.println(curModeInt);
  }
  lastStateRotarySW = currentStateRotarySW;

  currentStateRotaryCLK = digitalRead(rotaryCLK);
  if (currentStateRotaryCLK != lastStateRotaryCLK && currentStateRotaryCLK == 1) {
    if (digitalRead(rotaryDT) != currentStateRotaryCLK) {
      if (curModeInt == 4) {
        alarmTime = alarmTime - TimeSpan(0,0,1,0);
        Serial.println("alarm --");
      } else {
        rtc.adjust(rtc.now() - TimeSpan(0,0,1,0));
        Serial.println("clock --");
      }
    } else {
      if (curModeInt == 0) {
        alarmTime = alarmTime + TimeSpan(0,0,1,0);
        Serial.println("alarm ++");
      } else {
        rtc.adjust(rtc.now() + TimeSpan(0,0,1,0));
        Serial.println("clock ++");
      }
    }
  }
  lastStateRotaryCLK = currentStateRotaryCLK;
}

void setAlarm() {
  Serial.println("button pressed");
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
