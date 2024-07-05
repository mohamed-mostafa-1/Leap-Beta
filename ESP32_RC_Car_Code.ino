#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <Wire.h>
#include "time.h"
#include <Preferences.h>
#include <Arduino.h>
#include <ESP32Servo.h>
#include <LiquidCrystal_I2C.h>

// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Define pins for the Ultrasonic Sensor
#define echoPin 2
#define trigPin 4

#define IR_RIGHT 32
#define IR_BACK 33
#define IR_LEFT 34

long duration;
float distance;

LiquidCrystal_I2C lcd(0x27, 16, 2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

// Custom character for Tesla logo (simple representation)
byte teslaLogo[8] = {
  0b00000,
  0b00100,
  0b01110,
  0b10101,
  0b00100,
  0b01110,
  0b10101,
  0b00000
};

Servo myServo;  // create servo object to control a servo

float pos = 0;  // variable to store the servo position
// Insert your network credentials
#define WIFI_SSID "******"
#define WIFI_PASSWORD "*****"

// Insert Firebase project API Key
#define API_KEY "***********************"

// Insert RTDB URL
#define DATABASE_URL "*****"
/*Define the Firebase Data object */
FirebaseData fbdo;
/*Define the FirebaseAuth data for authentication data */
FirebaseAuth auth;
/*Define the FirebaseConfig data for config data */
FirebaseConfig config;
// Delaying time
unsigned long dataMillis = 0;
// Declaring IR SENSOR 

//Bluetooth signal Store in this variable
int FBSignal1;
int FBSignal2;

//initial Speed
int Speed = 100;

//declare channel for pwm Output
#define R 0
#define L 1

//PWM Pin for Controlling the speed
int enA = 12;
int enB = 13;

//motor controlling pin
int IN1 = 17;
int IN2 = 16;
int IN3 = 19;
int IN4 = 18;

void setup() {
  Serial.begin(115200);
  
  // Initialize servo first to ensure proper setup
  myServo.attach(15);
  
  lcd.init();
  lcd.clear();
  lcd.backlight();
  
  lcd.setCursor(4, 0);  
  lcd.print("L e a p");

  lcd.setCursor(6, 1);  
  lcd.print("BETA");
  
  lcd.setCursor(7, 0);  
  lcd.write(byte(0));   

  pinMode(IR_RIGHT, INPUT);
  pinMode(IR_BACK, INPUT);
  pinMode(IR_LEFT, INPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
  
  config.api_key = API_KEY;
  auth.user.email = "**************";
  auth.user.password = "***********";
  config.database_url = DATABASE_URL;
  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);
  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);

  ledcSetup(R, 5000, 8);  // Channel 0 for Motor A, 5 kHz frequency, 8-bit resolution
  ledcAttachPin(enA, R);
  ledcSetup(L, 5000, 8);  // Channel 1 for Motor B, 5 kHz frequency, 8-bit resolution
  ledcAttachPin(enB, L);

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
  unsigned long currentMillis = millis();
  if (Firebase.ready() && (currentMillis - dataMillis > 100)) {
    dataMillis = currentMillis;

    // Read analog data from an IR sensor
    int IR_reads1 = analogRead(IR_RIGHT);
    int IR_reads2 = analogRead(IR_LEFT);
    int IR_reads3 = analogRead(IR_BACK);

    // Debug prints for raw analog readings
    Serial.print("IR_RIGHT Raw: ");
    Serial.println(IR_reads1);
    Serial.print("IR_LEFT Raw: ");
    Serial.println(IR_reads2);
    Serial.print("IR_BACK Raw: ");
    Serial.println(IR_reads3);

    // Map the analog readings to a 0-200 cm range
    int IR_mapped1 = map(IR_reads1, 0, 4095, 0, 200);
    int IR_mapped2 = map(IR_reads2, 0, 4095, 0, 200);
    int IR_mapped3 = map(IR_reads3, 0, 4095, 0, 200);
    
    // Debug prints for mapped readings
    Serial.print("IR_RIGHT Mapped: ");
    Serial.println(IR_mapped1);
    Serial.print("IR_LEFT Mapped: ");
    Serial.println(IR_mapped2);
    Serial.print("IR_BACK Mapped: ");
    Serial.println(IR_mapped3);
    
    // Update Firebase with new sensor data
    updateFirebase("IR_RIGHT/Real_Reads", IR_reads1);
    updateFirebase("IR_RIGHT/Mapped_Reads", IR_mapped1);
    updateFirebase("IR_LEFT/Real_Reads", IR_reads2);
    updateFirebase("IR_LEFT/Mapped_Reads", IR_mapped2);
    updateFirebase("IR_BACK/Real_Reads", IR_reads3);
    updateFirebase("IR_BACK/Mapped_Reads", IR_mapped3);
    
    if (Firebase.RTDB.getInt(&fbdo, "/Speed/speed", &FBSignal1)) {
      Serial.print("Speed: ");
      Serial.println(FBSignal1);
    } else {
      Serial.println(fbdo.errorReason());
    }

    if (Firebase.RTDB.getInt(&fbdo, "/Directions/directions", &FBSignal2)) {
      Serial.print("Direction: ");
      Serial.println(FBSignal2);
    } else {
      Serial.println(fbdo.errorReason());
    }

    adjustSpeedAndDirection(FBSignal1, FBSignal2);
  }

  distance = measureDistance();

  if (distance > 0) {
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
  }

  if (distance < 20) {
    findDistance();
  }
}

void updateFirebase(const char* path, int value) {
  if (!Firebase.RTDB.setInt(&fbdo, path, value)) {
    Serial.print("Failed to update ");
    Serial.print(path);
    Serial.print(": ");
    Serial.println(fbdo.errorReason());
  }
}

void adjustSpeedAndDirection(int speed, int direction) {
  Speed = speed;

  switch (direction) {
    case 1: forward(); break;
    case 2: backward(); break;
    case 3: left(); break;
    case 6: right(); break;
    case 0: stop(); break;
    default: stop(); break;
  }
}

void backward() {
  ledcWrite(R, Speed);
  ledcWrite(L, Speed);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void forward() {
  ledcWrite(R, Speed);
  ledcWrite(L, Speed);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void left() {
  ledcWrite(R, Speed);
  ledcWrite(L, Speed);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void right() {
  ledcWrite(R, Speed);
  ledcWrite(L, Speed);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void stop() {
  ledcWrite(R, 0);
  ledcWrite(L, 0);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

float measureDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH, 30000);

  float distance = duration * 0.034 / 2;

  if (distance >= 2 && distance <= 400) {
    return distance;
  } else {
    return 0;
  }
}

void findDistance() {
  for (pos = 0; pos <= 180.0; pos += 0.5) {
    myServo.write(pos);
    delay(15);
    
    distance = measureDistance();

    if (distance > 0) {
      Serial.print("Scanning... Distance: ");
      Serial.print(distance);
      Serial.println(" cm");
      Firebase.RTDB.setFloat(&fbdo, "Servo/servo", distance);
      Firebase.RTDB.setFloat(&fbdo, "UltraSonic/ultrasonic", pos);
    }

    if (distance > 20) {
      return;
    }
  }
}
