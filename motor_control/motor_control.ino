#include<Servo.h>
Servo servo;
// give a name to digital pin 2, which has a pushbutton attached
const int pushButton = 2;
//red button
const int pushButtonRed = 5;

// the transistor which controls the motor will be attached to digital pin 9
const int motorControl = 9;
const int motorControlR = 10;

const int trigPin = 3;
const int echoPin = 4;
int distance, new_distance, duration;

int distVector[5] = {1000,1000,1000,1000,1000};
int index = 0;
bool stop = false;
bool running = false;
const int debug = 12;
int test = 0;

//controling the spacetime
int time = 0;
int waitTime = 0;
int stage = 0;

int angle = 0;

// the setup routine runs once when you press reset:
void setup() {
  servo.attach(11);
  servo.write(angle); 

  pinMode(pushButtonRed, INPUT);

  pinMode(debug, OUTPUT); //diode
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
  if(digitalRead(pushButtonRed) == HIGH) {
    stage = 0;
    time = 0;
    waitTime = 20;
    angle = 0;
  }
  if (!wait()){
    switch (stage){
      case 0:
        //Czekaj na przycisk - rozpoczęcie robienia kawy
        servo.write(0);
        if(digitalRead(pushButton) == HIGH){
          analogWrite(motorControl, 255);
          delay(50);
          Serial.println("0->1");
          stage = 1;
        } else {
          analogWrite(motorControl,0);
          analogWrite(motorControlR,0);
        }
        delay(1); 
        break;
      case 1:
        //Obniż do wykrycia kubka
        stop = false;
        //detecting interruption
        for(int a = 0; a < 5; a++) {
          if(distVector[a] > 7) {
            stop = false;
            break;
          } else {
            stop = true;
          }
        }
        if(stop) {
          analogWrite(motorControl,0);
          Serial.println("1->2");
          stage = 2;
        }
        
        break;
      case 2:
        //Wlej wodę
        if (angle < 120) {
          servo.write(angle);
          angle++;
          waitTime = 2;
        } else if (angle == 120) {
          servo.write(angle);
          angle++;
          waitTime = 100;
        } else if (angle <= 240) {
            servo.write(120-(angle-120));
            angle++;
            waitTime = 2;
        } else {
          angle = 0;
          stage = 3;
          waitTime = 1000; // Poczekaj minutę (nie wiem czy to dokładnie 60000)
          Serial.println("2->3");
        }
        break;
      case 3:
        
        //Zamieszaj
        stage = 4;
        waitTime = 1000; // Poczekaj 10 sekund (przy założeniu że mieszanie trwa 5 sekund)
        Serial.println("3->4");
        break;
      case 4:
        //Obniż
        analogWrite(motorControl,255);
        stage = 5;
        Serial.println("4->5");
        waitTime = 1000; 
        break;
      case 5:
        // Poczekaj 2 minuty aż się przeleje
        analogWrite(motorControl,0);
        stage = 6;
        Serial.println("5->6");
        waitTime = 1000;
        break;
      case 6:
        //Podnieś
        analogWrite(motorControlR,255);
        Serial.println("6->0");
        stage = 0;
        waitTime = 1000;
        break;
    }
  }
  
  // // read the state of the button and check if it is pressed
  // if(digitalRead(pushButton) == HIGH){
  //   analogWrite(motorControl, 255);
  //   delay(150);
  // }
  // delay(1);        // delay in between reads for stability
  // stop = false;
  // //detecting interruption
  // for(int a = 0; a < 5; a++) {
  //   if(distVector[a] > 10) {
  //     stop = false;
  //     break;
  //   } else {
  //     stop = true;
  //   }
  // }
  

  // //servo added with pouring water and coffee
  // delay(60000);
  // //string coffee - another servo
  // delay(15000);
  // // down
  // delay(120000);
  // //wait until everything goes through
  
  // if(stop) {
  //   analogWrite(motorControl,0);
  //   delay(100);
  //   for(int x = 0; x <= 255; x+=10){
  //     analogWrite(motorControlR, 255);
  //     delay(50);
  //   }
  //   delay(100);
  //   analogWrite(motorControlR,0);
  // }


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
  // Serial.print("Distance: ");
  // Serial.print(
  //     distance); // Print the output in serial monitor
  // Serial.println(" cm");

  if(index == 5) {
    index = 0;
  }

  distVector[index++] = distance;


  return distance;
}


bool wait(){
  if (time == waitTime) {
    time = 0;
    waitTime = 0;
    return false;
  } else {
    time ++;
    return true;
  }
}