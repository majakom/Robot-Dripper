const int debug = 13;
const int motion9 = 9;
const int motion8 = 8;
int distance;
int new_distance;
const int sensitivity = 20;

void setup() {
  pinMode(debug, OUTPUT);
  pinMode(motion9, OUTPUT);
  pinMode(motion8, INPUT);
  distance = measureDistance();

}

void loop() {
  new_distance = measureDistance();
  if(abs(distance - new_distance) > sensitivity) {
    digitalWrite(debug, HIGH);
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
