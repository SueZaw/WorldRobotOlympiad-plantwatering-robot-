#include <AFMotor.h>  // For adding the library of shield 
#include <Servo.h> 
#include <Wire.h>  // Include Wire library for I2C 
 
const int I2C_ADDRESS = 8;  // I2C address of this Arduino 
 
// Define pins for ultrasonic sensor 
const int trigPin1 = A3; 
const int echoPin1 = A4; 
 
// Initialize motor and servo objects 
AF_DCMotor motor1(1, MOTOR12_1KHZ); 
AF_DCMotor motor4(4, MOTOR12_1KHZ); 
AF_DCMotor motor3(3, MOTOR12_1KHZ);  // Water pump motor 
Servo m1;  // Servo motor 1 
Servo m2;  // Servo motor 2 
 
// Global variables for sensor data and state 
// int soilSensorData = 0;  // Variable to store soil sensor data 
bool pottedPlantDetected = false;  // Flag to track potted plant detection 
int distance = 0;  // Variable to store ultrasonic sensor distance 
 
void setup() { 
  Serial.begin(115200);  // For debugging 
  Wire.begin(I2C_ADDRESS);  // Initialize I2C as slave with address 8 
  // Wire.onReceive(receiveEvent);  // Register receive event 
 
  pinMode(trigPin1, OUTPUT); 
  pinMode(echoPin1, INPUT); 
 
  m1.attach(10);  // Define the attached pin for servo motor 1 
  m2.attach(9);   // Define the attached pin for servo motor 2 
 
  m1.write(0); 
  m2.write(120); 
 
  motor1.setSpeed(100);  // Set speed for motor 1 
  motor4.setSpeed(100);  // Set speed for motor 4 
  motor3.setSpeed(150);  // Set speed for the water pump motor 
} 
 
void loop() { 
  distance = readUltrasonicDistance();  // Read the distance from ultrasonic sensor 
  Forward();  // Move the robot forward 
  delay(500);  
  Stop();  // Stop the robot 
  delay(1000);  
 
  // if (pottedPlantDetected) { 
    // if (distance < 20){
    Stop();  // Stop when a plant is detected 
    delay(500); 
     
    // if (distance < 40) {  // Remove semicolon here 
      delay(500); 
      servoF();  // Move servos forward 
      delay(1500);  // Allow time for servos to move 
 
      // if (soilSensorData > 0) { 
        // Water the plant if soil is dry 
        delay(2000); 
        motor3.run(FORWARD);  // Start the water pump 
        delay(3000);  // Run the pump for 3 seconds 
        motor3.run(RELEASE);  // Stop the water pump 
        delay(3000);  // Wait before moving servos back 
      // } 
 
      // Return servos and continue 
      servoB();   
      delay(2000); 
      Forward(); 
      delay(500); 
      Stop(); 
    // } 
  // } else { 
  //   // Keep moving forward if no plant detected 
  //   Forward();  // Move the robot forward 
  //   delay(500);  
  //   Stop();  // Stop the robot 
  //   delay(1000);  
  // }  // Close the else block properly 
 
}
void receiveEvent(int howMany) { 
  if (Wire.available() >= 2) { 
    char c = Wire.read();  // Read the received byte 
    Serial.print("Received: "); 
    Serial.println(c); 
 
    if (c == 'P') {  // If 'P' is received, potted plant detected 
      pottedPlantDetected = true; 
      Serial.println("Potted plant detected!"); 
    } else if (c == 'N') {  // If 'N' is received, no plant detected 
      pottedPlantDetected = false; 
      Serial.println("Potted plant not detected!"); 
    // } else {  // Otherwise, it's part of the soil sensor data 
      // soilSensorData = c << 8;  // Read high byte 
      // soilSensorData |= Wire.read();  // Read low byte 
      // Serial.print("Soil Sensor Data: "); 
      // Serial.println(soilSensorData); 
    } 
  } 
} 
 
 int readUltrasonicDistance() { 
  // Trigger the ultrasonic sensor 
  digitalWrite(trigPin1, LOW); 
  delayMicroseconds(2); 
  digitalWrite(trigPin1, HIGH); 
  delayMicroseconds(10); 
  digitalWrite(trigPin1, LOW); 
 
  // Read the echo pin 
  long duration = pulseIn(echoPin1, HIGH); 
  int distance = duration * 0.0344 / 2;  // Convert to cm 
  Serial.print("Distance Sensor Data: "); 
  Serial.println(distance); 
  return distance; 
} 
 
void Stop() { 
  Serial.println("-----------  \n Stop!\n -----------"); 
  motor1.run(RELEASE); 
  motor4.run(RELEASE); 
} 
 
void Forward() { 
  Serial.println("-----------  \n Moving Forward!\n -----------"); 
  motor1.run(FORWARD); 
  motor4.run(FORWARD); 
} 
 
void servoF() { 
  Serial.println("Moving servos forward"); 
  m1.write(120);  // Move servo m1 to 120
 
  m2.write(0);    // Move servo m2 to 0 degrees 
  delay(2500);    // Delay to allow the servos to move 
} 
 
void servoB() { 
  Serial.println("Moving servos back"); 
  m1.write(0);    // Move servo m1 to 0 degrees 
  m2.write(120);  // Move servo m2 to 120 degrees 
  delay(2500);    // Delay to allow the servos to move 
}
