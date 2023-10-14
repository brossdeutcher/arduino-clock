#include <RTClib.h>
#include <TM1637Display.h>
#include "cantina.h"

// Red LED
#define redLED 3
// buzzer
#define buzzer 4
// Rotary Encoder
#define rotarySW 8
#define rotaryDT 9
#define rotaryCLK 10
int rotaryCounter = 0;
int currentStateRotaryCLK;
int lastStateRotaryCLK;
String rotaryCurrentDir = "";
unsigned long rotaryLastButtonPress = 0;
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


void setup() {
  // Red LED
  pinMode(redLED, OUTPUT);
  // Seven Segment Display
  sevSegDisplay.clear();
  sevSegDisplay.setBrightness(7);
  sevSegDisplay.clear();
  sevSegDisplay.setSegments(alrm);
  delay(1000);
  // Rotary Encoder
  pinMode(rotarySW, INPUT_PULLUP);
  pinMode(rotaryDT, INPUT);
  pinMode(rotaryCLK, INPUT);
  Serial.begin(9600);
  lastStateRotaryCLK = digitalRead(rotaryCLK);
  // RTC
  rtc.begin();
  rtc.adjust(DateTime(2023, 9, 7, 7, 59, 58)); // boot time
}

void loop() {
  now = rtc.now();

  sevSegHandler();
  rotaryEncoderHandler();

  if (now.hour() == 8 && now.minute() == 0 && now.second() < 10) {
    playCantina(redLED, buzzer);
  }
}

void rotaryEncoderHandler() {
  
  // TODO: fix rotary encoder (sometimes mistakes which direction it is going, might be broken encoder)
  // TODO: set alarm with rotary encoder
  
  currentStateRotaryCLK = digitalRead(rotaryCLK);
  if (currentStateRotaryCLK != lastStateRotaryCLK && currentStateRotaryCLK == 1) {
    if (digitalRead(rotaryDT) != currentStateRotaryCLK) {
      rtc.adjust(rtc.now() - TimeSpan(0,0,1,0));
      int hrMin = (now.hour() * 100) + now.minute();
      Serial.print(hrMin);
      Serial.println(" min --");
    } else {
      rtc.adjust(rtc.now() + TimeSpan(0,0,1,0));
      int hrMin = (now.hour() * 100) + now.minute();
      Serial.print(hrMin);
      Serial.println(" min ++");
    }
  }
  lastStateRotaryCLK = currentStateRotaryCLK;
}

void sevSegHandler() {
  sevSegDisplay.showNumberDecEx(now.minute(), 0, true, 2, 2);
  sevSegDisplay.showNumberDecEx(now.hour(), 0b01000000, true, 2, 0);
}
