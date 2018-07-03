//////////////////////////////////////////////////////////////////
//©2011 bildr
//Released under the MIT License – Please reuse change and share
//Using the easy stepper with your arduino
//use rotate and/or rotateDeg to controll stepper motor
//speed is any number from .01 -> 1 with 1 being fastest –
//Slower Speed == Stronger movement
/////////////////////////////////////////////////////////////////

#define DIR_PIN 8
#define STEP_PIN 9
int limitSwitch1 = 7;
int limitSwitch2 = 4;
long incomingByte = 0;
long totalInput = 0;
long count = 0;
long inputs[10];
boolean isNegative = false;
int conversionRate = 400;
long distance = 0;
long len = 46400; //Length that the motor can go

void setup() {
  pinMode(DIR_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  Serial.begin(9600);
  pinMode(limitSwitch1, INPUT);
  pinMode(limitSwitch2, INPUT);

  //Find Start
  while (digitalRead(limitSwitch1) == HIGH) {
    rotate(-1, 1);
  }
  rotate(1, 1);
  distance = 0;

}

//1600 = 4 mm? -- 400 = 1 mm?
//Length = 46400 = 116 mm ---- 114.3 mm?
//1600 steps = one full revolution
void loop() {


  incomingByte = 0;
  //Resetting Values:
  for (int i = 0; i < sizeof(inputs) / sizeof(long); i++) {
    inputs[i] = 0;
  }
  count = 0;
  totalInput = 0;
  isNegative = false;
  incomingByte = 0;

  while (incomingByte != 10) {

    if (Serial.available() > 0) {

      incomingByte = Serial.read();
      //Serial.print("if: ");
      //Serial.println(incomingByte);
      if (incomingByte > 64)
        break;
      if (incomingByte != 10) {
        inputs[count] = incomingByte;
        count += 1;
      }
    }
  }
  if (incomingByte > 64) {
    rotate((distance * -1), 1);
    distance = 0;
  }
  count = 0;
  for (int i = ((sizeof(inputs) / sizeof(long)) - 1); i >= 0; i--) {
    if (inputs[i] > 47) {
      inputs[i] -= 48;
      inputs[i] *= (int)power(10, count);
      totalInput += inputs[i];
      count += 1;
    }
    else if (inputs[i] == 45) {
      isNegative = true;
    }
  }
  if (isNegative == true) {
    totalInput *= -1;
  }
  totalInput *= conversionRate;
  //Serial.print("Done: ");
  //Serial.println(totalInput);
  if (totalInput <= len && totalInput >= 0) {
    totalInput -= distance;
    rotate(totalInput, 1);
    //distance += totalInput;
  } else if (totalInput > len) {
    Serial.print("Too big. The most you can enter is ");
    Serial.print(len / conversionRate);
    Serial.println(" mm.");
  } else {
    Serial.println("Too small. You must enter a positive number.");
  }
  //Serial.println(distance);
}
long power(long base, long exponent) {
  long result = 1;
  for (long i = 0; i < exponent; i++) {
    result *= base;
  }
  return result;
}

void rotate(long steps, float speed) {
  //rotate a specific number of microsteps (8 microsteps per step) – (negitive for reverse movement)
  //speed is a90ny number from .01 -> 1 with 1 being fastest – Slower is stronger
  if (steps < 0) {
    isNegative = true;
  } else {
    isNegative = false;
  }
  long dir = (steps > 0) ? HIGH : LOW;
  steps = abs(steps);
  digitalWrite(DIR_PIN, dir);
  float usDelay = (1 / speed) * 70;
  for (long i = 0; i < steps; i++) {
    if (digitalRead(limitSwitch1) == LOW) {
      i = steps;
      delay(2);
      rotateLimit(1, 1);
    } else if (digitalRead(limitSwitch2) == LOW) {
      i = steps;
      delay(2);
      rotateLimit(-1, 1);
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
    if(digitalRead(limitSwitch1) == LOW||digitalRead(limitSwitch2)==LOW){
      steps+=1;
      /*if(digitalRead(limitSwitch1)==LOW){
        Serial.println("1");
      }else if(digitalRead(limitSwitch2)==LOW){
        Serial.println("2");
      }*/
    }
    tempDistance += 1;
  }
  tempDistance *= negMult;
  distance += tempDistance;
}

void rotateNonStop(int steps, float speed) {
  //rotate a specific number of microsteps (8 microsteps per step) – (negitive for reverse movement)
  //speed is any number from .01 -> 1 with 1 being fastest – Slower is stronger
  int dir = (steps > 0) ? HIGH : LOW;
  steps = abs(steps);

  digitalWrite(DIR_PIN, dir);

  float usDelay = (1 / speed) * 70;

  for (int i = 0; i < steps; i++) {
    //Serial.print("NOW");
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(usDelay);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(usDelay);
  }
}



void rotateDeg(float deg, float speed) { //rotate a specific number of degrees (negitive for reverse movement) //speed is any number from .01 -> 1 with 1 being fastest – Slower is stronger
  long dir = (deg > 0) ? HIGH : LOW;
  digitalWrite(DIR_PIN, dir);

  long steps = abs(deg) * (1 / 0.225);
  float usDelay = (1 / speed) * 70;

  for (int i = 0; i < steps; i++) {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(usDelay);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(usDelay);
  }
}
