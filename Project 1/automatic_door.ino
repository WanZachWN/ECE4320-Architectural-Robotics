#include "Arduino.h"
#include "Servo.h"

// Pin Definitions
#define HCSR04_PIN_TRIG  6
#define HCSR04_PIN_ECHO 5
#define PUSHBUTTON_PIN_2  7
#define RGBLED_PIN_B  2
#define RGBLED_PIN_G  3
#define RGBLED_PIN_R  4
#define SERVO9G_PIN_SIG 8

// Global variables and defines
const int servo9gRestPosition   = 20;  //Starting position
const int servo9gTargetPosition = 155; //Position when event is detected

// object initialization
int button_state = 0;
int last_button_state = 0;
int door_state = 0;
long duration, cm;
Servo servo9g;

void setup() {
  // put your setup code here, to run once:
  
  //servo
  servo9g.attach(SERVO9G_PIN_SIG);
  servo9g.write(servo9gRestPosition);
  //RGB LED
  pinMode(RGBLED_PIN_R, OUTPUT);
  pinMode(RGBLED_PIN_G, OUTPUT);
  pinMode(RGBLED_PIN_B, OUTPUT);
  //Button
  pinMode(PUSHBUTTON_PIN_2, INPUT);
  //Ultrasonic Sensor
  pinMode(HCSR04_PIN_TRIG, OUTPUT);
  pinMode(HCSR04_PIN_ECHO, INPUT);
  
  Serial.begin(9600);
  digitalWrite(RGBLED_PIN_R, 255);
  digitalWrite(RGBLED_PIN_G, 0);
  digitalWrite(RGBLED_PIN_B, 0);
  
  door_state = servo9g.read();
  if(door_state != servo9gRestPosition)
  {
    for(int i = door_state; i >= servo9gRestPosition; i -= 5)
    {
      servo9g.write(i);
      delay(100);
      Serial.print("door_state: ");
      Serial.println(i);
    }
  }
  
}

void loop() {
  // put your main code here, to run repeatedly:

  digitalWrite(HCSR04_PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(HCSR04_PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(HCSR04_PIN_TRIG, LOW);
  duration = pulseIn(HCSR04_PIN_ECHO, HIGH);
  cm = microsecondsToCentimeters(duration);
  Serial.print("cm: ");
  Serial.println(cm);
  
  button_state = digitalRead(PUSHBUTTON_PIN_2);
  door_state = servo9g.read(); 
  
  //open door
  if((cm <= 5) || (button_state == HIGH) && (door_state == servo9gRestPosition))
  {
    last_button_state = button_state;
    digitalWrite(RGBLED_PIN_R, 0);
    digitalWrite(RGBLED_PIN_G, 255);
    digitalWrite(RGBLED_PIN_B, 0);

    for(int i = 25; i <= servo9gTargetPosition; i += 5)
    {
      servo9g.write(i);
      delay(100);
    }
    delay(8000);

  }
  //someone by the door, extend time
  else if(((cm <= 15) || (button_state == HIGH)) && (door_state == servo9gTargetPosition) )
  {
    Serial.println("delay");
    delay(8000);
  }
  //close door
  else if (((cm > 15) || (last_button_state == HIGH)) && (door_state == servo9gTargetPosition))
  {
    for(int i = servo9gTargetPosition; i >= servo9gRestPosition; i -= 5)
    {
      servo9g.write(i);
      digitalWrite(RGBLED_PIN_R, 0);
      digitalWrite(RGBLED_PIN_G, 0);
      digitalWrite(RGBLED_PIN_B, 0);
      delay(100);
      digitalWrite(RGBLED_PIN_R, 255);
      digitalWrite(RGBLED_PIN_G, 0);
      digitalWrite(RGBLED_PIN_B, 0);
      delay(100);
      digitalWrite(HCSR04_PIN_TRIG, LOW);
      delayMicroseconds(2);
      digitalWrite(HCSR04_PIN_TRIG, HIGH);
      delayMicroseconds(10);
      digitalWrite(HCSR04_PIN_TRIG, LOW);
      duration = pulseIn(HCSR04_PIN_ECHO, HIGH);
      cm = microsecondsToCentimeters(duration);
      Serial.print("cm: ");
      Serial.println(cm);
      if(cm <= 5)
      {
        break;
      }
    }
  }
  
}

long microsecondsToCentimeters(long microseconds) {
   return microseconds / 29 / 2;
}
