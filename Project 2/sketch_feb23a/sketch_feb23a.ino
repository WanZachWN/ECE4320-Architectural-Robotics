#include <FastLED.h>

#include "Arduino.h"
#include "SD.h"
#include "TMRpcm.h"
#include "SPI.h"
#include "HX711.h"

#define LED_STRIP_PIN  2       //Blue pin of the RGB LED
#define LOADCELL_DOUT_PIN  3
#define LOADCELL_SCK_PIN  4
#define SD_ChipSelectPin 5

#define NUM_LEDS 40

CRGB leds[NUM_LEDS];
TMRpcm tmrpcm;
int i;
uint8_t hue = 0;
HX711 scale(DOUT, CLK);

void setup() {
  // put your setup code here, to run once:
  //LED
  Serial.println("resetting");
  LEDS.addLeds<WS2812,LED_STRIP_PIN,RGB>(leds,NUM_LEDS);
  LEDS.setBrightness(84);

  tmrpcm.speakerPin = 9;
  Serial.begin(9600);
  if (!SD.begin(SD_ChipSelectPin))
  {
    Serial.println("SD fail");
    return;
  }
  
}

void fadeall() { for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(250); } }

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available() > 0)
  {
    i = Serial.read();
    Serial.print("I received: ");
    Serial.println(i);
  }
  if(i == 1)
  {
    for(int i = 0; i < NUM_LEDS; i++) {
      // Set the i'th led to red 
      leds[i] = CHSV(hue++, 255, 255);
      // Show the leds
      FastLED.show(); 
      // now that we've shown the leds, reset the i'th led to black
      // leds[i] = CRGB::Black;
      fadeall();
      // Wait a little bit before we loop around and do it again
      delay(70);
    }
    for(int i = (NUM_LEDS)-1; i >= 0; i--) {
      // Set the i'th led to red 
      leds[i] = CHSV(hue++, 255, 255);
      // Show the leds
      FastLED.show();
      // now that we've shown the leds, reset the i'th led to black
      // leds[i] = CRGB::Black;
      fadeall();
      // Wait a little bit before we loop around and do it again
      delay(70);
    }
    
  }
  else if(i == 2)
  {
    tmrpcm.setVolume(6);
    tmrpcm.play("**.wav");
  }
  else if(i == 3)
  {
    Serial.print("Reading: ");
    Serial.print(scale.get_units(), 1); //scale.get_units() returns a float
    Serial.print(" lbs"); //You can change this to kg but you'll need to refactor the calibration_factor
    Serial.println();
  }
}
