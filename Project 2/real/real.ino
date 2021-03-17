#include "Arduino.h"
#include "pitches.h"
#include <FastLED.h>
#include <Stepper.h>
#include <Q2HX711.h>

#define NUM_LEDS 40

const int led = 2;
const byte hx711_data_pin = 3;
const byte hx711_clock_pin = 4;
const int audioPin = 5;
const int motorPin1 = 8;                // IN1 pin on the ULN2003A driver
const int motorPin2 = 9;                // IN2 pin on the ULN2003A driver
const int motorPin3 = 10;               // IN3 pin on the ULN2003A driver
const int motorPin4 = 11;               // IN4 pin on the ULN2003A driver

CRGB leds[NUM_LEDS];
uint8_t hue = 0;

int currentWeight;
int readWeight;
Q2HX711 hx711(hx711_data_pin, hx711_clock_pin);

int stepsPerRevolution = 2048;        // steps per revolution
Stepper stepper(stepsPerRevolution, motorPin1, motorPin3, motorPin2, motorPin4);

//
int weightState = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);               // initialise the serial monitor
  
  stepper.setSpeed(15);            // set the current speed
  
  //LED
  LEDS.addLeds<WS2812,led,RGB>(leds,NUM_LEDS);
  LEDS.setBrightness(84);
  FastLED.clear();
  FastLED.show();

  //Speaker
  pinMode(audioPin, OUTPUT);

  //Weight Sensor
  currentWeight = GetWeight();
}
void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(currentWeight);
  readWeight = GetWeight();
  Serial.println(readWeight);
  if(readWeight > currentWeight+50 && weightState == 0)
  {
    weightState = 1;
    currentWeight = readWeight;
  }
  if(weightState == 1)
  {
    sing();
    StepperRev();
    weightState = 0;
    RGBOff();
  }
}

void RGBOn()
{
  hue=hue+10;
  fill_solid(leds, NUM_LEDS, CHSV(hue,255,255));
  FastLED.show();
}

void RGBOff()
{
  FastLED.clear();
  FastLED.show();
}

void sing() {
    Serial.println(" 'Mario Theme'");
    int size = sizeof(melody) / sizeof(int);
    for (int thisNote = 0; thisNote < size; thisNote++)
    {
      // to calculate the note duration, take one second
      // divided by the note type.
      //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
      int noteDuration = 1000 / tempo[thisNote];
      buzz(audioPin, melody[thisNote], noteDuration);
      // to distinguish the notes, set a minimum time between them.
      // the note's duration + 30% seems to work well:
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      // stop the tone playing:
      buzz(audioPin, 0, noteDuration);
    }
}

void buzz(int targetPin, long frequency, long length) {
  RGBOn();
  long delayValue = 1000000 / frequency / 2; // calculate the delay value between transitions
  //// 1 second's worth of microseconds, divided by the frequency, then split in half since
  //// there are two phases to each cycle
  long numCycles = frequency * length / 1000; // calculate the number of cycles for proper timing
  //// multiply frequency, which is really cycles per second, by the number of seconds to
  //// get the total number of cycles to produce
  for (long i = 0; i < numCycles; i++) { // for the calculated length of time...
    digitalWrite(targetPin, HIGH); // write the buzzer pin high to push out the diaphram
    delayMicroseconds(delayValue); // wait for the calculated delay value
    digitalWrite(targetPin, LOW); // write the buzzer pin low to pull back the diaphram
    delayMicroseconds(delayValue); // wait again or the calculated delay value
  } 
}

void StepperRev()
{
  stepper.step(stepsPerRevolution);
  delay(500);
  stepper.step(-stepsPerRevolution);
  delay(500);
}

int GetWeight()
{
  int weight = hx711.read()/100.0;
  return weight;
}
