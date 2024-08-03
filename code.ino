#define BLYNK_TEMPLATE_ID "TMPL3LVghil_N"
#define BLYNK_TEMPLATE_NAME "Car"
#define BLYNK_TEMPLATE_AUTH_TOKEN "kKxBF0l9te9PBbFOs3pfOTdl5Z0lLOyL"
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MPU6050.h>
#include <BlynkSimpleEsp32.h>

const char* ssid = "Paneer";  // Replace with your network SSID
const char* password = "Jivyantra";  // Replace with your network password

Adafruit_MPU6050 mpu;

// Motor pins
#define IN1 5
#define IN2 4
#define IN3 18
#define IN4 19

// Vibration sensor pin
#define VIBRATION_PIN 2

BlynkTimer timer;

void setup() {
  Serial.begin(115200);

  // Connect to local WiFi network
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.print(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Blynk.begin(BLYNK_TEMPLATE_AUTH_TOKEN, ssid, password);

  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  pinMode(VIBRATION_PIN, INPUT);

  Blynk.virtualWrite(V7, "Ready");

  timer.setInterval(1000L, sendSensorDataToBlynk);
}

void loop() {
  Blynk.run();
  timer.run();
}

BLYNK_WRITE(V8) {
  int value = param.asInt();
  if (value == 1) {
    handleForward();
  } else if (value == 2) {
    handleBackward();
  } else if (value == 3) {
    handleStop();
  }
}

void handleForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  Serial.println("Moving Forward");
}

void handleBackward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  Serial.println("Moving Backward");
}

void handleStop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  Serial.println("Stopped");
}

void sendSensorDataToBlynk() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  Blynk.virtualWrite(V0, g.gyro.x);
  Blynk.virtualWrite(V1, g.gyro.y);
  Blynk.virtualWrite(V2, g.gyro.z);

  Blynk.virtualWrite(V3, a.acceleration.x);
  Blynk.virtualWrite(V4, a.acceleration.y);
  Blynk.virtualWrite(V5, a.acceleration.z);

  int vibration = digitalRead(VIBRATION_PIN);
  Blynk.virtualWrite(V6, vibration);

  String data = "Gyro X: " + String(g.gyro.x) + ", Gyro Y: " + String(g.gyro.y) + ", Gyro Z: " + String(g.gyro.z) + "Acceleration X: " + String(a.acceleration.x) + ", Acceleration Y: " + String(a.acceleration.y) + ", Acceleration Z: " + String(a.acceleration.z) + ", Vibration: " + (vibration == HIGH ? "Detected" : "Not Detected");
  Serial.println(data);
}
