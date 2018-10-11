#include <NewPing.h>
#include <Servo.h>

// PINS
// Motor Controller Pins
static const int leftBack    = A2;
static const int leftFwd     = A3;
static const int rightBack   = A4;
static const int rightFwd    = A5;
static const int leftSpeed   = 5;
static const int rightSpeed  = 6;

// Ultrasonic Range Finder Pins
static const int ultraSonicTrigPin = A0;
static const int ultraSonicEchoPin = A1;
static const int servoPin  = 11;


static const int stopDistance  = 25;
static const int hazardDistance = stopDistance * 2;
static const int maxDistance   = 200;

static const int servoLeft   = 180;
static const int servoFwd    = 100;
static const int servoRight  = 0;
int servoPosition     = servoFwd;

int driveSpeed      = 200;
int slowSpeed       = 100;
int transitionDelay = 500;

int leftCM    = 0;
int centerCM  = 0;
int rightCM   = 0;


NewPing sonar(ultraSonicTrigPin, ultraSonicEchoPin, maxDistance);
Servo servo;


void setup() {
  Serial.begin(9600);
  servo.attach(servoPin);

  pinMode(leftBack, OUTPUT);
  pinMode(leftFwd, OUTPUT);
  pinMode(rightBack, OUTPUT);
  pinMode(rightFwd, OUTPUT);
  pinMode(leftSpeed, OUTPUT);
  pinMode(rightSpeed, OUTPUT);

  analogWrite(leftSpeed, driveSpeed);
  analogWrite(rightSpeed, driveSpeed);

  scan();
}

void loop() {
  centerCM = doPing();

  if(obstacleDetected()) {
    handleObstacle();
  }
  else if(hazardDetected()) {
    handleHazard();
  }
  else {
    driveFast();
    driveForward();
  }
}

boolean hazardDetected() {
  if (centerCM < hazardDistance && centerCM >= stopDistance) {
    return true;
  }
  else {
    return false;
  }
}

boolean obstacleDetected() {
  if (centerCM < stopDistance && centerCM > 0) {
    return true;
  }
  else {
    return false;
  }
}

void handleHazard() {
  driveSlow();
}

void handleObstacle() {
  stopDriving();
  driveSlow();
  delay(transitionDelay);
  driveBackward();
  delay(transitionDelay);
  stopDriving();
  scan();
  delay(transitionDelay);
  turnMostOpen();
  delay(transitionDelay);
}

void driveFast() {
  analogWrite(leftSpeed, driveSpeed);
  analogWrite(rightSpeed, driveSpeed);
}

void driveSlow() {
  analogWrite(leftSpeed, slowSpeed);
  analogWrite(rightSpeed, slowSpeed);
}

void leftForward() {
  digitalWrite(leftFwd, HIGH);
  digitalWrite(leftBack, LOW);
}

void rightForward() {
  digitalWrite(rightFwd, HIGH);
  digitalWrite(rightBack, LOW);
}

void leftBackward() {
  digitalWrite(leftBack, HIGH);
  digitalWrite(leftFwd, LOW);
}

void rightBackward() {
  digitalWrite(rightBack, HIGH);
  digitalWrite(rightFwd, LOW);
}

void leftStop() {
  digitalWrite(leftBack, LOW);
  digitalWrite(leftFwd, LOW);
}

void rightStop() {
  digitalWrite(rightBack, LOW);
  digitalWrite(rightFwd, LOW);
}

void driveForward() {
  leftForward();
  rightForward();
}

void driveBackward() {
  leftBackward();
  rightBackward();
}

void turnMostOpen() {
  if (leftCM >= rightCM) {
    turnLeft();
  }
  else {
    turnRight();
  }
}

void turnLeft() {
  leftBackward();
  rightForward();
}

void turnRight() {
  leftForward();
  rightBackward();
}

void stopDriving() {
  leftStop();
  rightStop();
}

void scan() {
  lookLeft();
  delay(transitionDelay);
  leftCM = doPing();
  
  lookRight();
  delay(transitionDelay * 1.5);
  rightCM = doPing();
  
  lookAhead();
  delay(transitionDelay);
}

int doPing() {
  unsigned int pingValue = sonar.ping();
  int pingDistance = pingValue / US_ROUNDTRIP_CM;
  
  Serial.print("Ping: ");
  Serial.print(pingDistance);
  Serial.println("cm: ");

  return pingDistance;
}

void lookLeft() {
  servo.write(servoLeft);
}

void lookRight() {
  servo.write(servoRight);
}

void lookAhead() {
  servo.write(servoFwd);
}
