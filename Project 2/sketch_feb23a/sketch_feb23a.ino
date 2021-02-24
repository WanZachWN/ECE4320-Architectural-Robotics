#include "Arduino.h"
#include "SD.h"
#include "TMRpcm.h"
#include "SPI.h"

#define RGBLED_PIN_B  2       //Blue pin of the RGB LED
#define RGBLED_PIN_G  3       //Green pin of the RGB LED
#define RGBLED_PIN_R  4       //Red pin of the RGB LED
#define SD_ChipSelectPin 5

TMRpcm tmrpcm;
int i;

void setup() {
  // put your setup code here, to run once:
  //RGB LED
  pinMode(RGBLED_PIN_R, OUTPUT);
  pinMode(RGBLED_PIN_G, OUTPUT);
  pinMode(RGBLED_PIN_B, OUTPUT);

  tmrpcm.speakerPin = 9;
  Serial.begin(9600);
  if (!SD.begin(SD_ChipSelectPin))
  {
    Serial.println("SD fail");
    return;
  }
  
}

void loop() {
  // put your main code here, to run repeatedly:
  i = Serial.read();
  if(i == 1)
  {
    //LED turns to green
    digitalWrite(RGBLED_PIN_R, 0);
    digitalWrite(RGBLED_PIN_G, 255);
    digitalWrite(RGBLED_PIN_B, 0);
    delay(100);
    digitalWrite(RGBLED_PIN_R, 0);
    digitalWrite(RGBLED_PIN_G, 0);
    digitalWrite(RGBLED_PIN_B, 255);
    delay(100);
    digitalWrite(RGBLED_PIN_R, 255);
    digitalWrite(RGBLED_PIN_G, 0);
    digitalWrite(RGBLED_PIN_B, 0);
    delay(100);
    digitalWrite(RGBLED_PIN_R, 0);
    digitalWrite(RGBLED_PIN_G, 255);
    digitalWrite(RGBLED_PIN_B, 255);
    delay(100);
  }
  else if(i == 2)
  {
    tmrpcm.setVolume(6);
    tmrpcm.play("**.wav");
  }
}
