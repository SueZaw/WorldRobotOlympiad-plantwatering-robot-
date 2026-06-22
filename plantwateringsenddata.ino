#include <AFMotor.h> // For adding the library of shield 
#include <Servo.h>
#include <Wire.h>  // Include Wire library for I2C

int pos; // Variable that will define the position of servos 
const int trigPin1 = A0; // Defines the trig pin of ultrasonic sensor 1st 
const int echoPin1 = A1; // Defines the echo pin of ultrasonic sensor 1st 
// const int trigPin1=A2; // Defines the trig pin of ultrasonic sensor 2nd 
// const int echoPin1=A3; // Defines the echo pin of ultrasonic sensor 2nd
int mpin = A3; // Defines the pin of moisture sensor pin
int mout; // Variable to store the value given by moisture sensor
long duration, duration1; // Variable that stores the duration value given by ultrasonic sensor
int distance, distance1; // Variable that stores the distance value calculated by the formula

AF_DCMotor motor1(1, MOTOR12_1KHZ);
AF_DCMotor motor2(2, MOTOR12_1KHZ); 
// Defines the frequency which will be given to motor 1 
// AF_DCMotor motor2(2, MOTOR12_1KHZ); // Defines the frequency which will be given to motor 2 
AF_DCMotor motor3(3, MOTOR12_1KHZ);
Servo m1;  // Variable of servo motor 1 
Servo m2; // Variable of servo motor 2

void Stop() {
  Serial.print("-----------  \n stop!\n -----------  ");
  motor1.run(RELEASE);
  motor2.run(RELEASE); 
}

void Forward() {
  Serial.print("-----------  \n Moving Straight !\n -----------  ");
  delay(100);
  motor1.run(FORWARD); 
  motor2.run(FORWARD); 
} 

void servoF() {
  Serial.println("Moving servos forward");
   delay(2300);
  // for (pos=0;pos<=120;pos+=1){
    //m1.write(pos); m2.write(120-pos); 
   //}
  m1.write(120); 
  // Move servo m1 to 120 degrees
  m2.write(0);
    // Move servo m2 to 0 degrees
  delay(1500);   // Delay to allow the servos to move
}

void servoB() {
  Serial.println("Moving servos back");
    delay(2300);
//for(pos=120;pos>=0;pos-=1) { { m1.write(pos); m2.write(120-pos); } delay(30); 
  m1.write(0);  
 // Move servo m1 to 0 degrees
 m2.write(120); // Move servo m2 to 120 degrees
  delay(1500);   // Delay to allow the servos to move
}


void setup() {
  Serial.begin(115200); // Starts serial communication with the Arduino and PC 
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);

  m1.attach(10); // Define the attached pin for servo motor 1 
  m2.attach(9);  // Define the attached pin for servo motor 2

  m1.write(0);
  m2.write(120);
 motor1.setSpeed(100); // Set a very low speed for motor 1
  motor2.setSpeed(100); // To set the particular speed of motor 2

  Wire.begin(8);                // Join I2C bus with address 8
  Wire.onReceive(receiveEvent); // Register receive event
  Wire.onRequest(requestEvent); // Register request event
}

void loop() {
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);
  duration1 = pulseIn(echoPin1, HIGH);
  distance1 = duration1 * 0.034 / 2;
  
  mout = analogRead(mpin);

  delay(300);

  Serial.print("\n\n SoilSensor : ");
  Serial.print(mout);
  Serial.print("%");
  Serial.print("\n Distance: ");
  Serial.println(distance1);
  delay(500);

  if (distance1 >= 46) { 
    Forward();
    delay(100); 
  } 
  else if (distance1 < 45) {
    Stop();
    delay(1500);
    //digitalWrite(trigPin1, LOW);
    //delayMicroseconds(2);
    //digitalWrite(trigPin1, HIGH);
    //delayMicroseconds(10);
    //digitalWrite(trigPin1, LOW);
    //duration1 = pulseIn(echoPin1, HIGH);
    //distance1 = duration1 * 0.034 / 2;
    //if (distance1 >= 6) { 
    servoF();
    delay(500); 
    //}

    mout = analogRead(mpin); 
if (mout >= 970) { 
    delay(2000);

    
    motor3.setSpeed(255);  // Set the speed for the water pump motor
    motor3.run(FORWARD);   // Start the water pump
    
    delay(1000); 
      Serial.print("Water pump activated\n");
    motor3.run(RELEASE); 
     delay(3500); 
    servoB();
    delay(1000); // Stop the water pump after the delay
    Forward();            // Continue moving forward
   
}
else if (mout <970){
   delay(1000); 
    servoB();
     delay(1000); 
    Forward(); 
}
  }
}

// Function that executes whenever data is received from master
void receiveEvent(int howMany) {
  while (0 < Wire.available()) {
    char c = Wire.read();      // Receive byte as a character
    Serial.print(c);           // Print the character
  }
  Serial.println();             // To newline
}

// Function that executes whenever data is requested from master
void requestEvent() {
  // Convert the soil sensor value to a 2-byte array
  byte highByte = (mout >> 8) & 0xFF;
  byte lowByte = mout & 0xFF;
  
  // Send the high and low bytes of the soil sensor value
  Wire.write(highByte);
  Wire.write(lowByte);
}
