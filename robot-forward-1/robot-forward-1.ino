#include <IRremote.h>
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

static const int irPin  = 9;

static const int DRIVE_MODE_SELF  = 0;
static const int DRIVE_MODE_IR    = 1;
int driveMode = DRIVE_MODE_IR;

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
IRrecv irrecv(irPin);
decode_results results;
unsigned long key_value = 0;

void setup() {
  Serial.begin(9600);
  servo.attach(servoPin);

  irrecv.enableIRIn();
  irrecv.blink13(true);

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

  irDriveMode();
  
  if(driveMode == DRIVE_MODE_SELF) {
    selfDriveMode();
  }
}


void irDriveMode() {
  if (irrecv.decode(&results)) {
    if (results.value == 0XFFFFFFFF) {
      results.value = key_value;
    }

    switch(results.value){
      case 0xFFA25D:          
        Serial.println("CH-");
        break;
      case 0xFF629D: // CH - Toggle Drive Mode
        Serial.println("CH - Toggle driveMode");

        Serial.println("driveMode: " + String(driveMode));

        if (driveMode == DRIVE_MODE_SELF) {
          driveMode = DRIVE_MODE_IR;
        }
        else {
          driveMode = DRIVE_MODE_SELF;
        }
      
        Serial.println("New driveMode: " + String(driveMode));
        break;
      case 0xFFE21D:
        Serial.println("CH+");
        break;
      case 0xFF22DD:
        Serial.println("|<<");
        break;
      case 0xFF02FD:
        Serial.println(">>|");
        break;  
      case 0xFFC23D:
        Serial.println(">|");
        break;               
      case 0xFFE01F:
        Serial.println("-");
        break;  
      case 0xFFA857:
        Serial.println("+");
        break;  
      case 0xFF906F:
        Serial.println("EQ");
        break;
      case 0xFF6897:
        Serial.println("0");
        break;  
      case 0xFF9867:
        Serial.println("100+");
        break;
      case 0xFFB04F:
        Serial.println("200+");
        break;
      case 0xFF30CF:
        Serial.println("1");
        turnLeft();
        break;
      case 0xFF18E7:
        Serial.println("2");
        driveForward();
        break;
      case 0xFF7A85:
        Serial.println("3");
        turnRight();
        break;
      case 0xFF10EF:
        Serial.println("4");
        break;
      case 0xFF38C7:
        Serial.println("5");
        driveBackward();
        break;
      case 0xFF5AA5:
        Serial.println("6");
        break;
      case 0xFF42BD:
        Serial.println("7");
        break;
      case 0xFF4AB5:
        Serial.println("8");
        break;
      case 0xFF52AD:
        Serial.println("9");
        break;
    }

    key_value = results.value;
    delay(100);
    irrecv.resume();
  }
  else {
    if (driveMode == DRIVE_MODE_IR) {
      stopDriving();
    }
  }
}

void selfDriveMode() {
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
    Serial.println("hazard detected: " + String(centerCM) + " CM");
    return true;
  }
  else {
    return false;
  }
}

boolean obstacleDetected() {
  if (centerCM < stopDistance && centerCM > 0) {
    Serial.println("obstacle detected: " + String(centerCM) + " CM");
    return true;
  }
  else {
    return false;
  }
}

void handleHazard() {
  driveSlow();
  driveForward();
}

void handleObstacle() {
  stopDriving();
  driveSlow();
  delay(transitionDelay);
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
  if (centerCM > leftCM && centerCM > rightCM) {
    turnLeft();
    turnLeft();
  }
  else if (leftCM >= rightCM) {
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
  
//  Serial.print("Ping: ");
//  Serial.print(pingDistance);
//  Serial.println("cm: ");

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
