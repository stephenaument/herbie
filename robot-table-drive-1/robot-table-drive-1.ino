#include <NewPing.h>
#include <Servo.h>

const int leftBack  = A2;
const int leftFwd   = A3;
const int rightBack = A4;
const int rightFwd  = A5;

const int eA        = 5;
const int eB        = 6;

const int ultraSonicTrigPin = A0;
const int ultraSonicEchoPin = A1;

const int servoPin  = 11;


const int stopDistance  = 25;
const int maxDistance   = 200;

const int servoLeft   = 180;
const int servoFwd    = 100;
const int servoRight  = 0;
int servoPosition     = servoFwd;

int driveSpeed      = 128;
int transitionDelay = 500;


unsigned int leftPingValue    = 0;
unsigned int centerPingValue  = 0;
unsigned int rightPingValue   = 0;

NewPing sonar(ultraSonicTrigPin, ultraSonicEchoPin, maxDistance);
Servo servo;

void setup() {
  Serial.begin(9600);
  servo.attach(servoPin);

  pinMode(leftBack, OUTPUT);
  pinMode(leftFwd, OUTPUT);
  pinMode(rightBack, OUTPUT);
  pinMode(rightFwd, OUTPUT);
  pinMode(eA, OUTPUT);
  pinMode(eB, OUTPUT);

  analogWrite(eA, driveSpeed);
  analogWrite(eB, driveSpeed);

  servo.write(servoLeft);
  delay(transitionDelay);
  servo.write(servoRight);
  delay(transitionDelay * 1.5);
  servo.write(servoFwd);
  delay(transitionDelay);  
}

void loop() {
  doPing();

  int centerCM = centerPingValue / US_ROUNDTRIP_CM;

  if (Serial.available()) {
    int dir = Serial.parseInt();

    Serial.println(dir);

    switch(dir) {
      case 1:
        Serial.println("Turning Left");
        turnLeft();
        break;
      case 2:
        Serial.println("Driving Foreward");
        driveForward();
        break;
      case 3:
        Serial.println("Driving Backward");
        driveBackward();
        break;
      case 4:
        Serial.println("Turning Right");
        turnRight();
    }
    delay(transitionDelay*0.4);
    stopDriving();
  }
  
//  if(centerCM < stopDistance && centerCM > 0) {
//    stopDriving();
//    delay(transitionDelay);
//    driveBackward();
//    delay(transitionDelay);
//    turnLeft();
//    delay(transitionDelay);
//  }
//  else {
//    driveForward();
//  }
}

void driveForward() {
  digitalWrite(leftFwd, HIGH);
  digitalWrite(leftBack, LOW);
  digitalWrite(rightFwd, HIGH);
  digitalWrite(rightBack, LOW);
}

void driveBackward() {
  digitalWrite(leftBack, HIGH);
  digitalWrite(leftFwd, LOW);
  digitalWrite(rightBack, HIGH);
  digitalWrite(rightFwd, LOW);
}

void turnLeft() {
  digitalWrite(leftFwd, LOW);
  digitalWrite(leftBack, HIGH);
  digitalWrite(rightFwd, HIGH);
  digitalWrite(rightBack, LOW);
}

void turnRight() {
  digitalWrite(leftFwd, HIGH);
  digitalWrite(leftBack, LOW);
  digitalWrite(rightFwd, LOW);
  digitalWrite(rightBack, HIGH);
}

void stopDriving() {
  digitalWrite(leftBack, LOW);
  digitalWrite(leftFwd, LOW);
  digitalWrite(rightBack, LOW);
  digitalWrite(rightFwd, LOW);
}

void doPing() {
  centerPingValue = sonar.ping();
//  Serial.print("Ping: ");
//  Serial.print(centerPingValue / US_ROUNDTRIP_CM);
//  Serial.println("cm: ");
}

void moveServo(int newPosition) {
  servo.write(newPosition);
}
