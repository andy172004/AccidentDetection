#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MPU6050.h>

const char* ssid = "ESP32_AP";
const char* password = "12345678";

WebServer server(80);
Adafruit_MPU6050 mpu;

// Motor pins
#define IN1 5
#define IN2 4
#define IN3 18
#define IN4 19

void setup() {
  Serial.begin(115200);
  
  // Set up Access Point
  WiFi.softAP(ssid, password);
  Serial.println("Access Point started");
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());
  
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  server.on("/", handleRoot);
  server.on("/forward", handleForward);
  server.on("/backward", handleBackward);
  server.on("/stop", handleStop);
  server.on("/gyro", handleGyro);

  server.begin();

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  String html = "<html>\
  <head>\
  <title>Car Bot</title>\
  <style>\
    body { font-family: Arial, sans-serif; background-color: #f4f4f4; text-align: center; padding: 50px; }\
    h1 { color: #333; }\
    button {\
      padding: 15px 25px;\
      font-size: 24px;\
      margin: 10px;\
      cursor: pointer;\
      border-radius: 5px;\
      border: none;\
      color: #fff;\
    }\
    button#forward { background-color: #4CAF50; }\
    button#backward { background-color: #f44336; }\
    button#stop { background-color: #555; }\
    #gyro { font-size: 20px; }\
  </style>\
  </head>\
  <body>\
  <h1>Car Bot Control</h1>\
  <button id='forward' onclick=\"sendData('/forward')\">Move Forward</button>\
  <button id='backward' onclick=\"sendData('/backward')\">Move Backward</button>\
  <button id='stop' onclick=\"sendData('/stop')\">Stop</button>\
  <h2>Gyro Readings</h2>\
  <p id=\"gyro\">Loading...</p>\
  <script>\
    function sendData(url) {\
      fetch(url).then(response => response.text()).then(data => console.log(data));\
    }\
    setInterval(function() {\
      fetch('/gyro').then(response => response.text()).then(data => {\
        document.getElementById('gyro').innerHTML = data;\
      });\
    }, 1000);\
  </script>\
  </body>\
  </html>";
  server.send(200, "text/html", html);
}

void handleForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  Serial.println("Moving Forward");
  server.send(200, "text/plain", "Moving Forward");
}

void handleBackward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  Serial.println("Moving Backward");
  server.send(200, "text/plain", "Moving Backward");
}

void handleStop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  Serial.println("Stopped");
  server.send(200, "text/plain", "Stopped");
}

void handleGyro() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  String data = "Gyro X: " + String(g.gyro.x) + ", Gyro Y: " + String(g.gyro.y) + ", Gyro Z: " + String(g.gyro.z);
  Serial.println(data);
  server.send(200, "text/plain", data);
}
