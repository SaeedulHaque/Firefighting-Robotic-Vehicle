// Firefighting Robotic Vehicle - firmware, with two bug fixes (marked "fixed" below).
// The fixes have not been tested on the robot.
// Target: ATmega328P with Arduino bootloader, 16 MHz crystal.
// Pin numbers below are Arduino digital pin numbers.

#include <Servo.h>

Servo myservo;
int pos = 0;
boolean fire = false;

/*------- Inputs: flame sensors (digital out, LOW = flame detected) -------*/
#define Left_S    9   // left sensor
#define Right_S   10  // right sensor
#define Forward_S 8   // forward sensor

/*------- Outputs -------*/
#define LM1  2   // left motor  (L298N IN)
#define LM2  3   // left motor  (L298N IN)
#define RM1  4   // right motor (L298N IN)
#define RM2  5   // right motor (L298N IN)
#define pump 13  // 5V submersible water pump

void setup()
{
  pinMode(Left_S, INPUT);
  pinMode(Right_S, INPUT);
  pinMode(Forward_S, INPUT);

  pinMode(LM1, OUTPUT);
  pinMode(LM2, OUTPUT);
  pinMode(RM1, OUTPUT);
  pinMode(RM2, OUTPUT);
  pinMode(pump, OUTPUT);

  myservo.attach(11);  // SG90 nozzle servo
  myservo.write(90);   // centre position
}

void put_off_fire()
{
  delay(300);

  // Stop the robot
  digitalWrite(LM1, HIGH);
  digitalWrite(LM2, HIGH);
  digitalWrite(RM1, HIGH);
  digitalWrite(RM2, HIGH);

  // Start spraying
  digitalWrite(pump, HIGH);
  delay(1000);

  // Sweep the nozzle 50 -> 130 degrees
  for (pos = 50; pos <= 130; pos += 1) {
    myservo.write(pos);
    delay(10);
  }

  // Sweep back 130 -> 50 degrees
  // (fixed: the original version started this loop at 50, so it never ran)
  for (pos = 130; pos >= 50; pos -= 1) {
    myservo.write(pos);
    delay(10);
  }

  digitalWrite(pump, LOW);
  myservo.write(90);
  fire = false;
}

void loop()
{
  myservo.write(90);  // Sweep_Servo();

  if (digitalRead(Left_S) == 1 && digitalRead(Right_S) == 1 && digitalRead(Forward_S) == 1)
  {
    // Fire not detected: do not move the robot
    digitalWrite(LM1, HIGH);
    digitalWrite(LM2, HIGH);
    digitalWrite(RM1, HIGH);
    digitalWrite(RM2, HIGH);
  }
  else if (digitalRead(Forward_S) == 0)
  {
    // Fire straight ahead: move forward
    digitalWrite(LM1, HIGH);
    digitalWrite(LM2, LOW);
    digitalWrite(RM1, HIGH);
    digitalWrite(RM2, LOW);
    fire = true;
  }
  else if (digitalRead(Left_S) == 0)
  {
    // Fire to the left: turn left
    digitalWrite(LM1, HIGH);
    digitalWrite(LM2, HIGH);
    digitalWrite(RM1, LOW);
    digitalWrite(RM2, HIGH);
    delay(500);
  }
  else if (digitalRead(Right_S) == 0)
  {
    // Fire to the right: turn right (mirror of the left turn)
    // (fixed: the original version set all four pins HIGH, which brakes instead of turning)
    digitalWrite(LM1, LOW);
    digitalWrite(LM2, HIGH);
    digitalWrite(RM1, HIGH);
    digitalWrite(RM2, HIGH);
    delay(500);
  }

  delay(50);  // slow down the speed of the robot

  while (fire == true)
  {
    put_off_fire();
  }
}
