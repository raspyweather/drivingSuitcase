#include <AccelStepper.h>
#include <MultiStepper.h>

/* Pins of the stepper driver shield*/
#define dirPinLeft 3
#define stepperPinLeft 2
#define dirPinRight 7
#define stepperPinRight 6

/* Maximum speed of the steppers, 
depends if you're using microsteps
and your desired motor speed */
#define maxMotorSpeed 300

/* Maximum acceleration; if set too high your motors might stall or your suitcase turns over */
#define maxAcceleration 100
/* Used to configure the motors as driver-based steppers */
#define motorDriver 1

AccelStepper leftMotor(motorDriver, stepperPinLeft, dirPinLeft);
AccelStepper rightMotor(motorDriver, stepperPinRight, dirPinRight);


int leftInput, rightInput;
void setupMotors(){
  leftMotor.setMaxSpeed(maxMotorSpeed);
  leftMotor.setAcceleration(maxAcceleration);
  rightMotor.setMaxSpeed(maxMotorSpeed);
  rightMotor.setAcceleration(maxAcceleration);
}
void setup() {
  Serial.begin(115200);
  setupMotors();
}
String input;

/* read serial input and adjust motors target.*/
void updateSpeed() {
  if (!Serial.available()) {
    return;
  }
  char inputChar = Serial.read();
  if (inputChar == '\n' && input.length() >= 10)
  {
    leftInput =  input.substring(1, 5).toInt();
    rightInput =  input.substring(6, 11).toInt();
    input = "";
    leftMotor.move(leftInput);
    rightMotor.move(rightInput);
  }
  else if(inputChar == '\n' && input.length() < 10)
  {
    /* faulty input */
    input = "";
  }
  else {
    input += inputChar;
  }
}

void loop() {
  leftMotor.run();
  rightMotor.run();
  updateSpeed();
}