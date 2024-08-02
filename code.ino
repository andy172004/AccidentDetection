#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MPU6050.h>

const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

WebServer server(80);
Adafruit_MPU6050 mpu;

// Motor pins
#define IN1 5
#define IN2 4
#define IN3 18
#define IN4 19

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());

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
  </head>\
  <body>\
  <h1>Car Bot Control</h1>\
  <button onclick=\"location.href='/forward'\">Move Forward</button>\
  <button onclick=\"location.href='/backward'\">Move Backward</button>\
  <button onclick=\"location.href='/stop'\">Stop</button>\
  <h2>Gyro Readings</h2>\
  <p id=\"gyro\">Loading...</p>\
  <script>\
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
  server.send(200, "text/html", "Moving Forward... <a href='/'>Go Back</a>");
}

void handleBackward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  server.send(200, "text/html", "Moving Backward... <a href='/'>Go Back</a>");
}

void handleStop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  server.send(200, "text/html", "Stopped... <a href='/'>Go Back</a>");
}

void handleGyro() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  String data = "Gyro X: " + String(g.gyro.x) + ", Gyro Y: " + String(g.gyro.y) + ", Gyro Z: " + String(g.gyro.z);
  server.send(200, "text/plain", data);
}
