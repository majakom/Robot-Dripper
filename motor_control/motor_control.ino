
// give a name to digital pin 2, which has a pushbutton attached
const int pushButton = 2;

// the transistor which controls the motor will be attached to digital pin 9
const int motorControl = 9;
const int motorControlR = 10;

const int trigPin = 3;
const int echoPin = 4;
int distance, duration;

int distVector[5] = {1000,1000,1000,1000,1000};
int index = 0;
bool stop = false;
bool running = false;

// the setup routine runs once when you press reset:
void setup() {
  // make the pushbutton's pin an input:
  pinMode(pushButton, INPUT);

  // make the transistor's pin an output:
  pinMode(motorControl, OUTPUT);  
  pinMode(motorControlR, OUTPUT);
  
  //defining trigger pin in the distance detector
  pinMode(trigPin, OUTPUT);

  //defining echo pin in the distance detector
  pinMode(echoPin, INPUT);

  Serial.begin(9600);
  Serial.println(
      "Distance measurement using Arduino Uno.");
  delay(500);

}

// the loop routine runs over and over again forever:
void loop() {
  
  distance = measureDistance();
  delay(100);
  // read the state of the button and check if it is pressed
  if(digitalRead(pushButton) == HIGH){
    // ramp up the motor speed
    //for(int x = 0; x <= 255; x+=10){
      analogWrite(motorControl, 255);
      delay(150);
      
    //}
    running = true;
    // ramp down the motor speed
    
   
  }
  delay(1);        // delay in between reads for stability
  stop = false;
  for(int a = 0; a < 5; a++) {
    if(distVector[a] > 10) {
      stop = false;
      break;
    } else {
      stop = true;
    }
  }

  if(stop && running) {
    analogWrite(motorControl,0);
    delay(100);
    for(int x = 0; x <= 255; x+=10){
      analogWrite(motorControlR, 255);
      delay(50);
    }
    delay(100);
    analogWrite(motorControlR,0);
    running = false;
  }


  delay(1);

}


int measureDistance() {

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin,HIGH);
  distance = duration*0.0344/2;
  Serial.print("Distance: ");
  Serial.print(
      distance); // Print the output in serial monitor
  Serial.println(" cm");

  if(index == 5) {
    index = 0;
  }

  distVector[index++] = distance;


  return distance;
}