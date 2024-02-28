#include <Arduino.h>
#include <Servo.h> //servo header file

//Ultrasonic sensor pin assignments
const int trigPins[] = {4, 6};
const int echoPins[] = {2, 3}; 
const int numSensors = sizeof(trigPins) / sizeof(trigPins[0]); //2 sensors
uint32_t distance;

//DC Motor pins
const int dcMotor1 = 10; // PWM DC motor directional pin
const int dcMotor2 = 11;  // PWM DC motor directional pin
int speed = 0;

//Servo pin
const int servo1Pin = 9;
Servo servo1; //creates servo1 object

void setup() {
  // Initailization of communication hardware (subject to change, this is the RF24  
  Serial.begin(9600); // setting serial communication rate to 115200 meant for setup of serial comm between Arduino and RPi
  // Wait for initial position from Raspberry Pi
  while (!Serial.available()) {
    delay(100);
  }
  
  // Initialization of all motors (DC, Servo)
  pinMode(dcMotor1, OUTPUT); //driving motor
  pinMode(dcMotor2, OUTPUT); //driving motor
  servo1.attach(servo1Pin); //rudder control

  //Ultrasonic sensor setup, trying to consolidate code
  for(int i = 0; i < numSensors; i++){
    pinMode(trigPins[i], OUTPUT);
    pinMode(echoPins[i], INPUT);
  }
}

void Operation(){
  //uint32_t distance;
  long dur[numSensors]; //timers to determine how long it takes a signal to travel from the ultrasonic sensor and back again
  //need to figure out what logic is needed to begin pool sweep cycle and what will conclude pool sweep
  servo1.write(90);
  digitalWrite(dcMotor1, HIGH);
  speed = 25;
  analogWrite(dcMotor1, speed);
  digitalWrite(dcMotor2, LOW);
  //ultra sonic sensors
  for(int i = 0; i < numSensors; i++){
    digitalWrite(trigPins[i], LOW);
    delayMicroseconds(2);
    digitalWrite(trigPins[i], HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPins[i], LOW);
    dur[i] = pulseIn(echoPins[i], HIGH);
    distance = (dur[i] * 0.034)/2; // this equation is (time * speed of sound as cm/ms)/ round trip of signal (think of the distance there and the distance back. If we only multiply by 0.034 we're essentially doubling the distance even though the signal only traveled x feet)

    if(distance <= 15){
      speed = 20;
      if(i == 0){ 
        while(distance <= 35){
          digitalWrite(dcMotor1, LOW); //stops forward drive of motor
          servo1.write(115); //turns towards ultra sonic sensor one
          analogWrite(dcMotor2, speed); //reverses motor
       }
        while(distance <= 50){
          digitalWrite(dcMotor2, LOW); //stops forward drive of motor
          servo1.write(35); //turns away from ultra sonic sensor one
          analogWrite(dcMotor1, speed); //reverses motor
        }
      }

      if(i == 1) {
        while(distance <= 35){
          digitalWrite(dcMotor1, LOW); //stops forward drive of motor
          servo1.write(55); //turns towards ultra sonic sensor one
          analogWrite(dcMotor2, speed); //reverses motor
        }
        while(distance <= 50){
          digitalWrite(dcMotor2, LOW); //stops forward drive of motor
          servo1.write(135); //turns away from ultra sonic sensor one
          analogWrite(dcMotor1, speed); //reverses motor
        }
      }
    }
  }
}

void loop(){
  Operation();
}