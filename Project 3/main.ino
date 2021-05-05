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
int count = 0;
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
  if(matState == 2 && armState == 0)
  {
    Serial.println("Stop increasing height");
    StopHeight();
  }
  else if(seatState == 0)
  {
    if(matState == 2 && armState == 1)
    {
      Serial.println("Going Up");
      HeightRiser();
      count++;
    }
  }
  else if(matState == 0 && armState == 0)
  {
    if(count != 0)
    {
      Serial.println("Going Down");
      HeightLower();
      count--;
    }
  }
  delay(500);
}
//====================================================================================
void SeatSensor()
{
  seatReading = analogRead(seatSensor);
  //Serial.print("Seat = ");
 // Serial.println(seatReading);
  if(seatReading > 200)
  {
    seatState = 1;
  }
  else
  {
    seatState = 0;
  }
  Serial.print("Seat = ");
  Serial.println(seatState);
}
//====================================================================================
void ArmRestSensor()
{
  leftArmReading = analogRead(leftArmRestSensor);
  
  if(leftArmReading > 70)
  {
    leftArmState = 1;
    delay(3000);
  }
  else
  {
    leftArmState = 0;
  }
  rightArmReading = analogRead(rightArmRestSensor);
  if(rightArmReading > 70)
  {
    rightArmState = 1;
  }
  else
  {
    rightArmState = 0;
  }
  
  if(leftArmState == 1 && rightArmState == 1)
  {
    armState = 1;
  }
  else
  {
    armState = 0; 
  }
  Serial.print("Left read = ");
  Serial.println(leftArmReading);
  Serial.print("Right read = ");
  Serial.println(rightArmReading);
  Serial.print("Left Arm = ");
  Serial.println(leftArmState);
  Serial.print("Right Arm = ");
  Serial.println(rightArmState);
  Serial.print("Arm = ");
  Serial.println(armState);
}
//====================================================================================
void MatSensor()
{
  matReading = analogRead(matSensor);
  //Serial.print("Mat = ");
  //Serial.println(matReading);
  if(matReading != 0)
  {
    if(matState == 0)
    {
      matState = 1;
      matMax = matReading;
    }
    if(matReading >= (0.75*matMax))
    {
      matState = 2;
    }
    else
    {
      matState = 1;
    }
  }
  else
  {
    matState = 0;
  }
  Serial.print("Mat = ");
  Serial.println(matState);
  Serial.print("Mat Reading = ");
  Serial.println(matReading);
  Serial.print("Mat Read = ");
  Serial.println(matMax);
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
