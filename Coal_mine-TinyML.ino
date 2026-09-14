// ==========================================
// BLYNK CREDENTIALS
// ==========================================
#define BLYNK_TEMPLATE_ID "Blynk_Temp_id"
#define BLYNK_TEMPLATE_NAME "Coal Mine Robot"
#define BLYNK_AUTH_TOKEN "Blynk_auth_token"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <Mine_AI_inferencing.h>

// ==========================================
// PINS
// ==========================================
#define MQ4_PIN 34
#define MQ7_PIN 35
#define DHTPIN 4
#define DHTTYPE DHT11

#define TRIG_PIN 5
#define ECHO_PIN 18
#define SERVO_PIN 19
#define BUZZER_PIN 23 

#define RED_LED 32
#define YELLOW_LED 13 
#define GREEN_LED 33
#define HEADLIGHT_PIN 12   // SAFE PIN

#define IN1 25
#define IN2 26
#define IN3 27
#define IN4 16 

// ==========================================
char ssid[] = "id_name";
char pass[] = "pass";

DHT dht(DHTPIN, DHTTYPE);
Servo radarServo;
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

bool autoMode = false;
bool isHazardActive = false;
bool headlightState = false;

unsigned long lastSensorRead = 0;
unsigned long navTimer = 0;
int navStep = 0;

// ==========================================
void setup() {
  Serial.begin(115200);

  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  stopMotors();

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(HEADLIGHT_PIN, OUTPUT);

  // ACTIVE LOW OFF
  digitalWrite(RED_LED, HIGH);
  digitalWrite(YELLOW_LED, HIGH);
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(HEADLIGHT_PIN, HIGH);

  radarServo.attach(SERVO_PIN);
  radarServo.write(90);

  dht.begin();
  accel.begin();

  setSafeState();

  // WiFi + Blynk (non-blocking)
  WiFi.begin(ssid, pass);
  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connect(5000);

  Blynk.virtualWrite(V11, 0); // headlight OFF

  Serial.println("System Ready");
}

// ==========================================
void loop() {

  Blynk.run();

  if (!Blynk.connected()) {
    Blynk.connect();
  }

  // Sensor system (slow task)
  if (millis() - lastSensorRead > 3000) {
    lastSensorRead = millis();
    runSensorSystem();
  }

  // Auto navigation
  if (autoMode && !isHazardActive) {
    runAutoNavigation();
  }
}

// ==========================================
// BLYNK CONTROL
// ==========================================
BLYNK_WRITE(V0) { autoMode = param.asInt(); if(!autoMode) stopMotors(); }

BLYNK_WRITE(V1) { autoMode = false; moveForward(); }
BLYNK_WRITE(V2) { autoMode = false; moveBackward(); }
BLYNK_WRITE(V3) { autoMode = false; turnLeft(); }
BLYNK_WRITE(V4) { autoMode = false; turnRight(); }

BLYNK_WRITE(V11) {
  headlightState = param.asInt();
  digitalWrite(HEADLIGHT_PIN, headlightState ? LOW : HIGH);
}

// ==========================================
// SENSOR + AI
// ==========================================
void runSensorSystem() {

  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  if (isnan(temp) || isnan(hum)) { temp = 30; hum = 60; }

  float methane = analogRead(MQ4_PIN);
  float co = analogRead(MQ7_PIN);

  float methane_norm = methane / 4095.0;
  float co_norm = co / 4095.0;

  // IMU
  sensors_event_t event;
  accel.getEvent(&event);

  float pitch = atan2(event.acceleration.y, event.acceleration.z) * 180.0 / PI;
  float roll = atan2(-event.acceleration.x,
                    sqrt(event.acceleration.y * event.acceleration.y + event.acceleration.z * event.acceleration.z)) * 180.0 / PI;

  Blynk.virtualWrite(V12, pitch);
  Blynk.virtualWrite(V13, roll);

  String orientation = (abs(pitch) < 20 && abs(roll) < 20) ? "UPRIGHT" : "TILTED";
  Blynk.virtualWrite(V20, orientation);

  // AI
  float data[4] = {methane_norm, co_norm, temp, hum};

  signal_t signal;
  if (numpy::signal_from_buffer(data, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal) != 0) return;

  ei_impulse_result_t result = {0};
  if (run_classifier(&signal, &result, false) != EI_IMPULSE_OK) return;

  float maxVal = 0;
  for (int i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
    if (result.classification[i].value > maxVal) {
      maxVal = result.classification[i].value;
    }
  }

  float mineRisk = (methane_norm + co_norm + maxVal) / 3.0;

  if (mineRisk > 0.75) triggerHazard("AI GAS");
  else if (mineRisk > 0.4) triggerWarning("ELEVATED");
  else { setSafeState(); isHazardActive = false; }

  Blynk.virtualWrite(V16, mineRisk * 100);
}

// ==========================================
// LED CONTROL
// ==========================================
void allLEDsOff() {
  digitalWrite(RED_LED, HIGH);
  digitalWrite(YELLOW_LED, HIGH);
  digitalWrite(GREEN_LED, HIGH);
}

void setSafeState() {
  allLEDsOff();
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  Blynk.virtualWrite(V10, "SAFE");
}

void triggerWarning(String reason) {
  allLEDsOff();
  digitalWrite(YELLOW_LED, LOW);

  digitalWrite(BUZZER_PIN, HIGH);
  delay(10);
  digitalWrite(BUZZER_PIN, LOW);

  Blynk.virtualWrite(V10, "WARNING: " + reason);
}

void triggerHazard(String reason) {
  isHazardActive = true;
  stopMotors();

  allLEDsOff();
  digitalWrite(RED_LED, LOW);

  digitalWrite(BUZZER_PIN, HIGH);
  delay(20);
  digitalWrite(BUZZER_PIN, LOW);

  Blynk.virtualWrite(V10, "DANGER: " + reason);
}

// ==========================================
// AUTO NAVIGATION (NON-BLOCKING)
// ==========================================
void runAutoNavigation() {

  if (millis() - navTimer < 150) return;
  navTimer = millis();

  static float front, left, right;

  switch (navStep) {

    case 0:
      radarServo.write(90);
      navStep++;
      break;

    case 1:
      front = getDistance();
      if (front > 25) {
        moveForward();
        navStep = 0;
      } else {
        stopMotors();
        navStep++;
      }
      break;

    case 2:
      radarServo.write(140);
      navStep++;
      break;

    case 3:
      left = getDistance();
      navStep++;
      break;

    case 4:
      radarServo.write(40);
      navStep++;
      break;

    case 5:
      right = getDistance();

      if (left > right && left > 25) turnLeft();
      else if (right > 25) turnRight();
      else moveBackward();

      navStep = 0;
      break;
  }
}

// ==========================================
// ULTRASONIC
// ==========================================
float getDistance() {

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 12000);

  if (duration == 0) return 300;

  float dist = duration * 0.034 / 2;

  if (dist < 2 || dist > 300) return 300;

  return dist;
}

// ==========================================
// MOTOR CONTROL
// ==========================================
void moveForward() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

void moveBackward() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
}

void turnLeft() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

void turnRight() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
}

void stopMotors() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}
