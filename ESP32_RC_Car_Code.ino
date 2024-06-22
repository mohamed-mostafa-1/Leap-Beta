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
#define WIFI_SSID "realme"
#define WIFI_PASSWORD "99999999"

// Insert Firebase project API Key
#define API_KEY "AIzaSyCGmKD7DbhuzGrDXoh_g5CrI_gFCk8d2IQ"

// Insert RTDB URL
#define DATABASE_URL "https://leapcariotproject-default-rtdb.firebaseio.com/"
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
  lcd.init();
  lcd.clear();
  lcd.backlight();
  
  lcd.createChar(0, teslaLogo);

  lcd.setCursor(5, 1);  
  lcd.print("Tesla");
  
  lcd.setCursor(7, 0);  
  lcd.write(byte(0));   

  myServo.attach(15);
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.begin(115200);

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
  auth.user.email = "abdelrahmantechcs@gmail.com";
  auth.user.password = "howareyou?!123";
  config.database_url = DATABASE_URL;
  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);
  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);

  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);

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
  if (Firebase.ready() && (millis() - dataMillis > 100)) {
    dataMillis = millis();

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

    switch (FBSignal1) {
      case 0: Speed = 100; break;
      case 1: Speed = 110; break;
      case 2: Speed = 120; break;
      case 3: Speed = 130; break;
      case 4: Speed = 140; break;
      case 5: Speed = 150; break;
      case 6: Speed = 180; break;
      case 7: Speed = 200; break;
      case 8: Speed = 220; break;
      case 9: Speed = 240; break;
      default: Speed = 100; break;
    }

    switch (FBSignal2) {
      case 2: backward(); break;
      case 8: forward(); break;
      case 4: left(); break;
      case 6: right(); break;
      case 0: stop(); break;
      default: stop(); break;
    }
  }

  distance = measur`eDistance();

  if (distance > 0) {
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
  }

  delay(1000);

  if (distance > 0 && distance < 20) {
    findDistance();
  }
}

// Functions for controlling the motor
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
