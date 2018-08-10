//////////////////////////////////////////////////////////////////
//©2011 bildr
//Released under the MIT License – Please reuse change and share
//Using the easy stepper with your arduino
//use rotate and/or rotateDeg to controll stepper motor
//speed is any number from .01 -> 1 with 1 being fastest –
//Slower Speed == Stronger movement
/////////////////////////////////////////////////////////////////
#include "params.h"

//byte that it recieves from Serial
float incomingByte = 0;
//number of steps that motor will move
float totalInput = 0;
//in order to fit incoming bytes into a number, they are first put into an array
long count = 0;
char inputs[15];
//if inputted number is negative, isNegative = true, steps *= -1
boolean isNegative = false;
//Steps/mm
float conversionRate = 402.8644;
//least amount able to be entered - 1 step
float least = 1./conversionRate;
//current distance from home
long distance = 0;
//Approximate number of steps the motor can go (a little over the estimated amount, so that the motor can get to the end - it will hit the limit switch if it goes too far anyways)
long len = 41500; 
//String that holds the serial input
String stringInput  = "";
//The speed that is used when rotating the motor
float speedNum = 1;
//boolean to represent when the user wants the motor to stay enabled at all times (and not resetto disable every time through the loop)
boolean enabled = false;
//float startLen = 6;  //approximate distance from edge of stage that moving stage starts at after going to "0"


void setup() {
  //Starting Serial Communication
  Serial.begin(9600);
  //Setting up motor pins as output
  pinMode(DIR_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);
  //Setting up limit switch pins as input
  pinMode(LIMIT_PIN1, INPUT);
  pinMode(LIMIT_PIN2, INPUT);
  //calling for the motor to home
  //homeMotor();
}

//114.3 mm?
//1600 steps = one full revolution
void loop() {
  if(enabled==false){
     digitalWrite(ENABLE_PIN, HIGH); //Disbales the motor, so it can rest untill it is called uppond
  }
  resetValues();
  //waiting for input
  while (incomingByte != 10) {
    if (Serial.available() > 0) {
      //Calling getInput(), which puts the incoming bytes into a string (into 'string input')
      getInput();
    }
  }
  
     //Check for "GoTo: "
    if(stringInput.substring(0,6).equalsIgnoreCase("GoTo: ")){
      totalInput = stringInput.substring(6).toFloat();
      goTo();
    }//Check for "Step: "
    else if(stringInput.substring(0,6).equalsIgnoreCase("Step: ")){
      totalInput = stringInput.substring(6).toFloat();
      stepMotor();
    }//Check for "Home"
    else if(stringInput.substring(0, 4).equalsIgnoreCase("Home")){
      homeMotor();
    }//Check for "Set Speed: "
    else if(stringInput.substring(0,11).equalsIgnoreCase("Set Speed: ")){
      speedNum = stringInput.substring(11).toFloat();
      Serial.print("Speed: ");
      Serial.println(speedNum);
    }//Check for "Get Speed"
    else if(stringInput.substring(0,9).equalsIgnoreCase("Get Speed")){
      Serial.print("Speed: ");
      Serial.println(speedNum);
    }//Check for "Get Position"
    else if(stringInput.substring(0,12).equalsIgnoreCase("Get Position")){
      Serial.print("Current Position: ");
      Serial.print((float)distance/conversionRate, 4);
      Serial.println(" mm.");
    }//Check for "Enable"
    else if(stringInput.substring(0,6).equalsIgnoreCase("Enable")){
      digitalWrite(ENABLE_PIN, LOW);
      enabled = true;
      Serial.println("Motor Enabled");
    }//Check for "Disable"
    else if(stringInput.substring(0,7).equalsIgnoreCase("Disable")){
      digitalWrite(ENABLE_PIN, HIGH);
      enabled = false;
      Serial.println("Motor Disabled");
    }//Check for "Get Enabled"
    else if(stringInput.substring(0, 11).equalsIgnoreCase("Get Enabled")){
      if(digitalRead(ENABLE_PIN)==HIGH){
        Serial.println("Motor is currently disabled.");
      }else{
        Serial.println("Motor is currently enabled.");
      }
    }//Check for "Get Status"
    else if(stringInput.substring(0, 10).equalsIgnoreCase("Get Status")){
      //Motor Enabled?
      if(digitalRead(ENABLE_PIN)==HIGH){
        Serial.println("Motor is currently disabled.");
      }else{
        Serial.println("Motor is currently enabled.");
      }
      //Current Position
      Serial.print("Current Position: ");
      Serial.print((float)distance/conversionRate, 4);
      Serial.println(" mm.");
      //Current Speed
      Serial.print("Speed: ");
      Serial.println(speedNum);
    }
   else if(stringInput.substring(0, 5).equalsIgnoreCase("Get Arduino")){
      Serial.println("Linear Stage Arduino Nano");
   }
    
}

//Goes to 'total distance' away from 0
void goTo(){
  totalInput *= conversionRate;
  if (totalInput <= len && totalInput >= 0) {
    totalInput -= distance;
    long steps = (long)totalInput - distance;
    rotate(totalInput, speedNum);
    delay(100);
    Serial.print("Position: ");
    Serial.println(((float)distance/conversionRate), 4);
  } else if (totalInput > len) {
    Serial.print("Too big. The most you can enter is ");
    Serial.print(len / conversionRate);
    Serial.println(" mm.");
  } else {
    Serial.print("Too small. You must enter a positive number.");
  }
}

void stepMotor(){
  totalInput *= conversionRate;
  totalInput += distance;
  if (totalInput <= len && totalInput >= 0) {
    totalInput -= distance;
    long steps = (long)totalInput - distance;
    rotate(totalInput, speedNum);
    delay(100);
    Serial.print("Position: ");
    Serial.println(((float)distance/conversionRate), 4);
  } else if (totalInput > len) {
    Serial.print("Too far. The most you can step at this time is ");
    Serial.print((len-distance)/conversionRate);
    Serial.println(" mm.");
  } else {
    Serial.print("Too small. The least that you can step at this time is ");
    Serial.print((distance/conversionRate)*-1);
    Serial.println(" mm.");
  }
}

void homeMotor(){
  digitalWrite(ENABLE_PIN, LOW); //Enabling the motor, so it will move when asked to
  delay(10);
  //Changing direction that motor will move
  digitalWrite(DIR_PIN, 0);
  //Find Start
  float firstDelay = (1/speedNum) * 70;
  while (digitalRead(LIMIT_PIN1) == HIGH) {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(firstDelay);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(firstDelay);
  }
  delay(3);
  rotate(1, speedNum);
  distance = 0;
  Serial.println("Homed");
}

void getInput(){
  incomingByte = Serial.read();
  if (incomingByte != 10) {
    //casting the incoming byte to a character and adding it to a string which holds the rest of the input
    stringInput += (char)incomingByte;
  }
}

void resetValues(){
  incomingByte = 0;
  //Resetting Values:
  for (int i = 0; i < sizeof(inputs) / sizeof(long); i++) {
    inputs[i] = 0;
  }
  count = 0;
  totalInput = 0;
  isNegative = false;
  incomingByte = 0;
  stringInput  = "";
}

long power(long base, long exponent) {
  long result = 1;
  for (long i = 0; i < exponent; i++) {
    result *= base;
  }
  return result;
}

void rotate(long steps, float speed) {
  digitalWrite(ENABLE_PIN, LOW); //Enabling the motor, so it will move when asked to
  //rotate a specific number of microsteps (8 microsteps per step) – (negitive for reverse movement)
  //speed is any number from .01 -> 1 with 1 being fastest – Slower is stronger
  delay(30); //importantDelay
  if (steps < 0) {
    isNegative = true;
  } else {
    isNegative = false;
  }
  int dir = (steps > 0) ? HIGH : LOW;
  steps = abs(steps);
  digitalWrite(DIR_PIN, dir);
  float usDelay = (1 / speed) * 70;
  for (long i = 0; i < steps; i++) {
    if (digitalRead(LIMIT_PIN1) == LOW) {
      i = steps;
      delay(10);
      rotateLimit(1, speedNum);
    } else if (digitalRead(LIMIT_PIN2) == LOW) {
      i = steps;
      delay(10);
      rotateLimit(-1, speedNum);
    } else {
      digitalWrite(STEP_PIN, HIGH);
      delayMicroseconds(usDelay);
      digitalWrite(STEP_PIN, LOW);
      delayMicroseconds(usDelay);
      if (isNegative == true) {
        distance -= 1;
      } else {
        distance += 1;
      }
      
    }
  }
}

void rotateLimit(long steps, float speed) {
  int negMult = 1;
  long tempDistance = 0;
  if(steps<0){
    negMult = -1;
  }
   int dir = (steps > 0) ? HIGH : LOW;
  steps = abs(steps);

  digitalWrite(DIR_PIN, dir);

  float usDelay = (1 / speed) * 70;

  for (int i = 0; i < steps; i++) {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(usDelay);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(usDelay);
    if(digitalRead(LIMIT_PIN1) == LOW||digitalRead(LIMIT_PIN2)==LOW){
      steps+=1;
    }
    tempDistance += 1;
  }
  tempDistance *= negMult;
  distance += tempDistance;
}
