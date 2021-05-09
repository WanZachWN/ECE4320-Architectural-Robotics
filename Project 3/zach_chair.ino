/**********************************************************************
 * 
 * References used to code this:
 * Linear Actuator control: https://www.progressiveautomations.com/blogs/how-to/how-to-use-relays-to-control-linear-actuators
 * Force Sensing Resistor:  https://learn.adafruit.com/force-sensitive-resistor-fsr/using-an-fsr
 * 
***********************************************************************/
//====================================================================================
//Declare the pins of sensors and actuators
//Analog Pins
const int leftArmRestSensor = 0;    //Analog pin 0 for left arm chair
const int rightArmRestSensor = 1;   //Analog pin 1 for right arm chair
const int seatSensor = 2;           //Analog pin 2 for seat of chair
const int matSensor = 3;            //Analog pin 3 for mat of chair

//Digital Pins
const int linearActuator1 = 7;      //Digital pin 7 for IN1 of Linear Actuator
const int linearActuator2 = 8;      //Digital pin 8 for IN1 of Linear Actuator
//========================================================================================================
//Declaration of Force Sensing Resistor reading variables
int seatReading = 0;        //Current FSR seat of chair reading variable
int leftArmReading = 0;     //Current FSR left arm of chair reading variable
int rightArmReading = 0;    //Current FSR right of chair reading variable
int matReading = 0;         //Current FSR mat of chair reading variable
int matMax = 0;             //Maximum mat FSR value variable (When user approach the chair save this)
int count = 0;              //Counter to lower the linear actuator variable
//========================================================================================================
//Declaration of current state for each sensor variables
int leftArmState = 0;       //Left arm current state variable
int rightArmState = 0;      //Right arm current state variable
int armState = 0;           //Both arm current state variable
int seatState = 0;          //seat current state variable
int matState = 0;           //mat current state variable
int heightState = 0;        //height current state variable
//========================================================================================================
void setup() {
  
  //Actuator Set Pin Mode to output
  pinMode(linearActuator1,OUTPUT);
  pinMode(linearActuator2,OUTPUT);

  //Left Arm Rest Sensor Set Pin Mode to input
  pinMode(leftArmRestSensor,INPUT);

  //Right Arm Rest Sensor Set Pin Mode to input
  pinMode(rightArmRestSensor,INPUT);

  //Seat Sensor Set Pin Mode to input
  pinMode(seatSensor,INPUT);

  //Mat Sensor Set Pin Mode to input
  pinMode(matSensor,INPUT);

  //Begin Serial
  Serial.begin(9600);
}
//============================================================================================================
void loop() {

  /*
   * Retrieve sensor values functions
   * MatSensor()     - get current mat sensor reading for the mat and assign a current state
   *                  on the mat.
   * ArmRestSensor() - get current arm sensor reading for the left and right arm. 
   *                   And assign a current state for the arms.
   * SeatSensor()    - get current seat sensor reading for the seat and assign a current state.
   */
  MatSensor();
  ArmRestSensor();
  SeatSensor();
  //-----------------------------------------------------------------------------------------------------------
  /*
   * If someone is already standing up. Stop increasing the height of the linear actuator.
   * Requirements:
   * Mat sensor current state is 2  - someone is standing with enough force: 75% of the force when
   *                                  initially approaching the mat.
   * Arm sensor current state is 0  - someone is not putting any amount of force onto both arm rest.
   * heightState                    - set height state to 1 and the person has already
   *                                  fully stand up (actuator height is up)
   */
  if(matState == 2 && armState == 0)
  {
    Serial.println("Stop increasing height");
    StopHeight();
    heightState = 1;
  }
  
  //-----------------------------------------------------------------------------------------------------------
  /*
   * If someone is attempting to stand up. Activate linear actuator to increase height.
   * Requirements:
   * Seat sensor current state is 0     - someone is not sitting on the seat
   * linear actuator current state is 0 - chair hasn't gone up and is at the floor.
   * Mat sensor current state is 2      - someone is attempting to stand with enough force: 75% of the force when
   *                                      initially approaching the mat.
   * Arm sensor current state is 1      - someone is putting enough force on both left and right arm rest)       
   * HeightRiser()                      - function to increase the linear actuator height.
   * count                              - increase counter + 10 every time HeightRiser() function is executed
   *                                    (this is due to low power when the actuator is going down, we have to 
   *                                    keep track of height increase and lower the actuator by an extra 10 
   *                                    or more times).
   */
  else if(seatState == 0 && heightState == 0)
  {
    if(matState == 2 && armState == 1)
    {
      Serial.println("Going Up");
      HeightRising();
      count = count+10;
    }
  }
  //-----------------------------------------------------------------------------------------------------------
  /*
   * If someone has gotten up, and are not standing on the mat. Activate linear actuator
   * to go down.
   * Requirements:
   * linear actuator current state is 1 - The actuator has fully gone up.
   * Mat sensor current state is 0      - someone is not standing or putting any pressure onto the mat.
   * Arm sensor current state is 0      - someone is not putting any pressure onto both arm rest.
   * count                              - decrease until count value approaches 0.
   * heightState                        - set height state to 0 when actuator is fully retract(all the way down).
   */
  if(heightState == 1)
  {
    if(matState == 0 && armState == 0)
    {
      if(count != 0)
      {
        Serial.println("Going Down");
        HeightLower();
        count--;
        if(count == 0)
        {
          heightState = 0;
        }
      }
    }
  }
  
  //apply delay because everything is too fast
  delay(500);
}
//==============================================================================================================
/*
 * Seat sensor for FSR on the seat of the chair. Read analog pin dedicated (Seat analog pin - 2).
 * If seat reading is more than the threshold (to prevent items placed onto the chair seat), set seat
 * state to 1 as someone is sitting on the chair.
 * 
 * If seat not then set seat state to 0 to indicate no one is currently sitting.
 */
void SeatSensor()
{
  seatReading = analogRead(seatSensor);
  Serial.print("Seat = ");
  Serial.println(seatReading);
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
//=============================================================================================================
/*
 * Arm sensor for FSR on the left and right arm rest of the chair. Read analog pin dedicated 
 * (Right analog pin - 1; Left analog pin - 0).
 * If both left and right arm rest does detect pressure on the sensor, then set arm state to 1 
 * to indicate someone is attempting to stand up.
 * 
 * If both left and right arm rest does not detect any pressure, then set arm rest state to 0. This also applies
 * if only one of them detects any force instead of the other (Safety precautions).
 */
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
//=============================================================================================================
/*
 * mat sensor for FSR of the chair. Read analog pin dedicated (mat analog pin - 3).
 * If the mat does detect pressure on the sensor, and the mat current state is equals to 0,
 * set the mat state to 1(someone is standing on it) and obtain the current pressure to be saved.
 *    - This is because when you are approaching to sit on the chair, you will have the highest force applied onto it.
 *    
 * If the mat does detect pressure but mat state is not equal to 0 (1 or 2), check if mat reading is more than 75% of the
 * maximum max value obtained.(Technically maximum mat reading can still change even if someone is putting little to no pressure.
 * Although this is due to the order of if statements but good enough that it works and too lazy to fix it... maybe one day :D ).
 * 
 * ***threshold could also be lowered instead of 75%... have to play around.
 * 
 * If mat does detect pressure and state is not zero or if not more than 75% of initial pressure. Then set the current state to 1.
 * 
 * If mat does not detect any pressure, then set mat state to 0 as no one is standing directly onto it.
 */
void MatSensor()
{
  matReading = analogRead(matSensor);
  Serial.print("Mat = ");
  Serial.println(matReading);
  if(matReading != 0)
  {
    if(matState == 0)
    {
      matState = 1;
      matMax = matReading;
    }
    else
    {
      if(matReading >= (0.75*matMax))
      {
        matState = 2;
      }
      else
      {
        matState = 1;
      }
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
//=============================================================================================================
/*
 * Extend linear actuator function.
 * Pin 7 (Red Wire) - set to Low
 * Pin 8 (Black Wire) - set to High
 */
void HeightRising()
{
  // Extend Linear Actuator
  digitalWrite(7, LOW);
  digitalWrite(8, HIGH);
}
//=============================================================================================================
/*
 * retracting linear actuator function.
 * Pin 7 (Red Wire) - set to High
 * Pin 8 (Black Wire) - set to Low
 */
void HeightLower()
{
  // Retracts Linear Actuator
  digitalWrite(7, HIGH);
  digitalWrite(8, LOW);
}
//=============================================================================================================
/*
 * stop extending or retracting linear actuator function.
 * Pin 7 (Red Wire) - set to High
 * Pin 8 (Black Wire) - set to High
 */
void StopHeight()
{
  // Stops Actuator
  digitalWrite(7, HIGH);
  digitalWrite(8, HIGH);
  
}
