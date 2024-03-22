#include<Servo.h>
Servo servo7;

int position7 = 0;
const int debug = 13;
const int motion9 = 9;
const int motion8 = 8;
int distance;
int new_distance;
const int sensitivity = 200;

void setup() {
  pinMode(debug, OUTPUT);
  pinMode(motion9, OUTPUT);
  pinMode(motion8, INPUT);
  distance = measureDistance();
  servo7.attach(7);

}

void loop() {
  servo7.write(0);
  new_distance = measureDistance();
  if(abs(distance - new_distance) > sensitivity) {
    digitalWrite(debug, HIGH);
    servo7.write(90);
    delay(1000);
    digitalWrite(debug, LOW);
  }
  distance = measureDistance();
  delay(50);
}

int measureDistance() {
  unsigned long duration = 0;

  digitalWrite (motion9, HIGH);
  delayMicroseconds (10);
  digitalWrite (motion9, LOW);
  duration = pulseIn (motion8, HIGH);

  int cmDis = duration * 0.03432 / 2;

  if (cmDis> 400) {
    cmDis = 400;
  }

  return cmDis;
}
