// Test program to run a robot equipped with the TB6612 motor driver board
// and configured as a standard Personal Robot - like Tito's robot - now called
// Zoxsor Alpha

//
// Left motor is connected to B outputs and right motor to A outputs

const int AIn1Pin = 7;   //  Right Motor Direction Controls
const int AIn2Pin = 6;   //
const int BIn1Pin = 9;   //  Left  Motor Direction Controls
const int BIn2Pin = 10;  //
const int STBYPin = 8;
const int APWMPin = 5;
const int BPWMPin = 11;
static const int MaxMotorChangePerSecond = 200;
static const int MotorAdjustmentInterval = 30;
static const int MaxMotorChangePerInterval = MaxMotorChangePerSecond * MotorAdjustmentInterval / 1000;

#include "TB6612Driver.h"

struct Stopwatch {
private:
  int _timeStarted;

public:
  void reset() {
    _timeStarted = millis();
  }

  bool elapsed(int duration) {
    if (_timeStarted == 0) {
      reset();
    } 
    return millis() - _timeStarted >= duration;
  }
};

struct Motors {
  int left;
  int right;
  
  bool operator==(const Motors& other) {
    return left == other.left && right == other.right;
  }
};

class Robot {
private:
  struct Step {
    Motors motors;
    int duration;
    Step* next;
  };

  Stopwatch stepTimer;
  Stopwatch motorTimer;
  Step* currentStep;
  Step* tailStep;

  void advanceStep() {
    if (stepTimer.elapsed(currentStep->duration) && motors == currentStep->motors) {
      Step* lastStep = currentStep;
      currentStep = currentStep->next;
      if (currentStep != NULL) {
        stepTimer.reset();
      }
      delete lastStep;
    }
  }

  void advanceMotors() {
    if (motorTimer.elapsed(MotorAdjustmentInterval)) {
      motors.left = nextMotorSpeed(motors.left, currentStep->motors.left);
      motors.right = nextMotorSpeed(motors.right, currentStep->motors.right);
      motorTimer.reset();
    }
  }

  int nextMotorSpeed(int currentSpeed, int targetSpeed) {
    int speedDiff = targetSpeed - currentSpeed;
    int direction = speedDiff / abs(speedDiff);
    return currentSpeed + min(abs(speedDiff), MaxMotorChangePerInterval) * direction;
  }

public:
  Motors motors;

  void pushStep(int motorLeft, int motorRight, int duration) {
    Step* step = new Step();
    step->motors.left = motorLeft;
    step->motors.right = motorRight;
    step->duration = duration;

    if (tailStep == NULL) {
      tailStep = step;
      currentStep = step;
    } else {
      tailStep->next = step;
      tailStep = tailStep->next;
    }
  }
  
  void tick() {
    if (currentStep == NULL) {
      return;
    }
    advanceStep();
    advanceMotors();
  }

  void dump() {
    if (currentStep == NULL) {
      return;
    }
    Serial.print("Step(mL:");
    Serial.print(currentStep->motors.left);
    Serial.print(", mR:");
    Serial.print(currentStep->motors.right);
    Serial.print(", d:");
    Serial.print(currentStep->duration);
    Serial.print("), Actual(mL:");
    Serial.print(motors.left);
    Serial.print(", mR:");
    Serial.print(motors.right);
    Serial.println(")");
  }
}; 


Robot* robot;
Stopwatch dumpTimer;

void setup() {
  Serial.begin(9600);

  pinMode(AIn1Pin, OUTPUT);  
  pinMode(AIn2Pin, OUTPUT);  
  pinMode(BIn1Pin, OUTPUT);  
  pinMode(BIn2Pin, OUTPUT);  
  
  digitalWrite(STBYPin, HIGH);
  SetDriveMotorsTB6612(0, 0);

  robot = new Robot();
  robot->pushStep(255, 255, 3000);
  robot->pushStep(255, 100, 5000);
  robot->pushStep(100, 255, 5000);
  robot->pushStep(0, 0, 1000);
  robot->pushStep(-255, 255, 5000);
  robot->pushStep(255, -255, 1000);
  robot->pushStep(0, 0, 1000);
}

void loop() {
  robot->tick();

  if (dumpTimer.elapsed(200)) {
    robot->dump();
    dumpTimer.reset();
  }

  SetDriveMotorsTB6612(robot->motors.left, robot->motors.right);
}

