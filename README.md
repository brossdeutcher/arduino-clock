# Arduino Clock
## A real-time on an Arduino

## Components
- Arduino Uno
- TM1637 Seven-Segment Display (TM1637Display.h)
- Rotary Encoder
- DS13707 Real Time Clock (RTClib.h)

## My Story
I was introduced to the Arduino through the 30 Days Lost in Space AdventureKit. Through this 30-day online course, I went from blinking an LED to using inputs for an OLED display.

After completing the course, I continued to learn more about using the Arduino and its components. I mostly played around with the different components to see what they could do and did displays on my OLED display.

This is my first project usable in the real world. I initially created this with only the components in my AdventureKit, but overnight, the clock would be a few minutes off. Determined to get a working clock, I bought an Adafruit DS1307 Real Time Clock chip. Because the pins were separated from the chip, I also got a soldering iron and learned how to solder the five pins. This also involved me having to find the RTClib library, read the documentation, and implement the software rather than relying on code from the course.

As of this writing, my clock has been running for a week and matches my laptop's clock to the second. I intend to expand this project into an alarm clock with a flashing LED and audio. I also started on the rotary encoder code in this repo with the intent of being able to adjust the time and set the alarm. Maybe someday, I will learn about 3D printing and give this project a creative case.

I intend on keeping this clock as my primary time keeper in my bedroom and am excited to add more functionality. Thank you for reading and I hope you enjoy this project as much as I did building it!

## Credit
https://github.com/robsoncouto/arduino-songs/blob/master/cantinaband/cantinaband.ino
