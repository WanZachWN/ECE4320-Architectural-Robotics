//https://www.progressiveautomations.com/blogs/how-to/how-to-use-relays-to-control-linear-actuators
//https://learn.adafruit.com/force-sensitive-resistor-fsr/using-an-fsr
//====================================================================================
//Declare the pins of sensors and actuators
//Analog
const int leftArmRestSensor = 0;
const int rightArmRestSensor = 1;
const int seatSensor = 2;
const int matSensor = 3;

//Digital
const int linearActuator1 = 7;
const int linearActuator2 = 8;
//====================================================================================
int seatReading = 0;
int leftArmReading = 0;
int rightArmReading = 0;
int matReading = 0;
int matMax = 0;
//====================================================================================
//state for each sensor
int leftArmState = 0;
int rightArmState = 0;
int armState = 0;
int seatState = 0;
int matState = 0;
//====================================================================================
void setup() {
  
  //Actuator Set Pin Mode and Inital Height
  pinMode(linearActuator1,OUTPUT);
  pinMode(linearActuator2,OUTPUT);

  //Left Arm Rest Sensor Set Pin Mode
  pinMode(leftArmRestSensor,INPUT);

  //Right Arm Rest Sensor Set Pin Mode and Inital Height
  pinMode(rightArmRestSensor,INPUT);

  //Seat Sensor Set Pin Mode and Inital Height
  pinMode(seatSensor,INPUT);

  //Mat Sensor Set Pin Mode and Inital Height
  pinMode(matSensor,INPUT);

  Serial.begin(9600);
}
//====================================================================================
void loop() {

  MatSensor();
  ArmRestSensor();
  SeatSensor();
  /*if(matState == 3 && armState == 0)
  {
    Serial.println("Stop increasing height");
    StopHeight();
  }
  else if(matState == 2 && armState == 1)
  {
    Serial.println("Going Up");
    HeightRiser();
  }
  else if((matState == 0 && seatState == 0)&& armState == 0)
  {
    Serial.println("Going Down");
    HeightLower();
  }*/
}
//====================================================================================
void SeatSensor()
{
  seatReading = analogRead(seatSensor);
  //Serial.print("Seat = ");
 // Serial.println(seatReading);
  if(seatReading > 500)
  {
    seatState = 1;
  }
}
//====================================================================================
void ArmRestSensor()
{
  leftArmReading = analogRead(leftArmRestSensor);
  Serial.print("Left Arm = ");
  Serial.println(leftArmReading);
  if(leftArmReading > 500)
  {
    leftArmState = 1;
    delay(3000);
  }
  
  rightArmReading = analogRead(rightArmRestSensor);
  //Serial.print("Right Arm = ");
  //Serial.println(rightArmReading);
  if(rightArmReading > 500)
  {
    rightArmState = 1;
  }

  if(leftArmState == 1 && rightArmState == 1)
  {
    armState = 1;
  }
}
//====================================================================================
void MatSensor()
{
  matReading = analogRead(matSensor);
  //Serial.print("Mat = ");
  //Serial.println(matReading);
  if(matReading > 500)
  {
    if(matState == 0)
    {
      matState = 1;
      matMax = matReading;
    }
    else if(matReading >= (0.75*matMax))
    {
      matState = 3;
    }
    else if(matReading >= (0.45*matMax))
    {
      matState = 2;
    }
  }
}
//====================================================================================
void HeightRiser()
{
  // Extend Linear Actuator
  digitalWrite(7, LOW);
  digitalWrite(8, HIGH);
}
//====================================================================================
void HeightLower()
{
  // Retracts Linear Actuator
  digitalWrite(7, HIGH);
  digitalWrite(8, LOW);
}
//====================================================================================
void StopHeight()
{
  // Stops Actuator
  digitalWrite(7, HIGH);
  digitalWrite(8, HIGH);
}
