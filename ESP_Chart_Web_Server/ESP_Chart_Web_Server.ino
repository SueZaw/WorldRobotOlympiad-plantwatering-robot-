#include <Wire.h> 
#include <ESP8266WiFi.h> 
#include <ESPAsyncWebServer.h> 
#include <LittleFS.h> 

const int soilSensorPin = A0;  // Analog pin for soil sensor 
const int I2C_ADDRESS = 8;     // I2C address for communication with Arduino 

// WiFi credentials 
const char* ssid = "ntrk"; 
const char* password = "Kopi@1406"; 
AsyncWebServer server(80); 

bool plantDetected = false;  // State variable to track plant detection status 

void setup() { 
  Serial.begin(115200); 
  Wire.begin(5, 4);  // Initialize I2C bus with SDA=5 and SCL=4 

  if (!LittleFS.begin()) { 
    Serial.println("Failed to mount LittleFS"); 
    return; 
  } 

  WiFi.begin(ssid, password); 
  Serial.print("Connecting to WiFi"); 
  int attempts = 0; 
  while (WiFi.status() != WL_CONNECTED && attempts < 30) { 
    delay(1000); 
    Serial.print("."); 
    attempts++; 
  } 
  if (WiFi.status() == WL_CONNECTED) { 
    Serial.println("\nConnected to WiFi"); 
    Serial.println(WiFi.localIP()); 
  } else { 
    Serial.println("\nFailed to connect to WiFi"); 
    return; 
  } 

  // Serve the index page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){ 
    if (LittleFS.exists("/index.html")) { 
      request->send(LittleFS, "/index.html"); 
    } else { 
      request->send(404, "text/plain", "File not found"); 
    } 
  }); 

  // Endpoint for plant detection signal
  server.on("/signal", HTTP_GET, [](AsyncWebServerRequest *request){ 
    plantDetected = true;  // Update state on detection 
    Serial.println("Potted plant detected!"); 
    request->send(200, "text/plain", "Potted plant detected successfully"); 
  }); 

  // Endpoint for no plant detection
  server.on("/no_signal", HTTP_GET, [](AsyncWebServerRequest *request){ 
    plantDetected = false;  // Update state to no plant detected 
    Serial.println("No potted plant detected!"); 
    request->send(200, "text/plain", "No potted plant detected successfully"); 
  }); 

  server.begin(); 
  Serial.println("HTTP server started"); 
} 

void loop() { 
  static unsigned long lastSend = millis(); 
  if (millis() - lastSend > 5000) {  // Send data every 5 seconds 
    lastSend = millis(); 

    // Send plant detection status 
    Wire.beginTransmission(I2C_ADDRESS); 
    Wire.write(plantDetected ? 'P' : 'N');  // Send 'P' if plant is detected, 'N' otherwise 
    Wire.endTransmission(); 

    // Log the status 
    Serial.print("Plant detection status: "); 
    Serial.println(plantDetected ? "Potted plant detected" : "No plant detected"); 
  } 
}
