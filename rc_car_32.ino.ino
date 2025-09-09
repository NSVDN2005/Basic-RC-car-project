#include <WiFi.h>
#include <WebServer.h>
#include <HardwareSerial.h>
#include <ESP32Servo.h>

// Motor A
int ENA = 26;
int IN1 = 14;
int IN2 = 27;

// Motor B
int IN3 = 25;
int IN4 = 33;
int ENB = 32;

const int wifiLedPin = 5;

int SPEED = 1023;
int speed_Coeff = 3;

WebServer server(80);

HardwareSerial SerialPort(2); // RX=16, TX=17
Servo myServo;

const int LED_PIN = 19;
const int SERVO_PIN = 18;
const int LDR_THRESHOLD = 800;

float humidity = 0.0;
int ldrValue = 0;
int currentServoAngle = 0;

void setup() {
  Serial.begin(115200);
  SerialPort.begin(15200, SERIAL_8N1, 16, 17);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(wifiLedPin, OUTPUT);
  digitalWrite(wifiLedPin, HIGH);

  myServo.attach(SERVO_PIN);
  pinMode(LED_PIN, OUTPUT);

  analogWrite(ENA, SPEED);
  analogWrite(ENB, SPEED);
  Stop();

  WiFi.mode(WIFI_AP);
  WiFi.softAP("ESP32_Car", "12345678");
  Serial.print("Access Point IP address: ");
  Serial.println(WiFi.softAPIP());
  digitalWrite(wifiLedPin, LOW);

  server.on("/", []() {
    String command = server.arg("State");

    if (command == "F") Forward();
    else if (command == "B") Backward();
    else if (command == "R") TurnRight();
    else if (command == "L") TurnLeft();
    else if (command == "G") ForwardLeft();
    else if (command == "H") BackwardLeft();
    else if (command == "I") ForwardRight();
    else if (command == "J") BackwardRight();
    else if (command == "S") Stop();
    else if (command == "0") SPEED = 330;
    else if (command == "1") SPEED = 400;
    else if (command == "2") SPEED = 470;
    else if (command == "3") SPEED = 540;
    else if (command == "4") SPEED = 610;
    else if (command == "5") SPEED = 680;
    else if (command == "6") SPEED = 750;
    else if (command == "7") SPEED = 820;
    else if (command == "8") SPEED = 890;
    else if (command == "9") SPEED = 960;
    else if (command == "q") SPEED = 1023;

    server.send(200, "text/plain", "Command: " + command);
  });

  server.onNotFound([]() {
    server.send(404, "text/plain", "Not found");
  });

  server.begin();
  Serial.println("Web server started.");
}

void loop() {
  server.handleClient();

  if (SerialPort.available()) {
    String humidityStr = SerialPort.readStringUntil('\n');
    humidityStr.trim();
    while (humidityStr.length() == 0 && SerialPort.available()) {
      humidityStr = SerialPort.readStringUntil('\n');
      humidityStr.trim();
    }

    String ldrStr = SerialPort.readStringUntil('\n');
    ldrStr.trim();
    while (ldrStr.length() == 0 && SerialPort.available()) {
      ldrStr = SerialPort.readStringUntil('\n');
      ldrStr.trim();
    }

    if (humidityStr.length() > 0 && ldrStr.length() > 0) {
      humidity = humidityStr.toFloat();
      ldrValue = ldrStr.toInt();

      Serial.print("Humidity: ");
      Serial.print(humidity);
      Serial.print(" %, LDR: ");
      Serial.println(ldrValue);

      // LED Control
      digitalWrite(LED_PIN, ldrValue > LDR_THRESHOLD ? HIGH : LOW);

      // Servo Control
      if (humidity >= 50 && humidity <= 100) {
        for (int angle = 0; angle <= 180; angle += 5) {
          myServo.write(angle);
          currentServoAngle = angle;
          delay(100);
        }
        for (int angle = 180; angle >= 0; angle -= 5) {
          myServo.write(angle);
          currentServoAngle = angle;
          delay(100);
        }
      }
    }
  }

  delay(200);
}

// Motor Control Functions
void Forward() {
  analogWrite(ENA, SPEED);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, SPEED);
}

void Backward() {
  analogWrite(ENA, SPEED);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENB, SPEED);
}

void TurnRight() {
  analogWrite(ENA, SPEED);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENB, SPEED);
}

void TurnLeft() {
  analogWrite(ENA, SPEED);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, SPEED);
}

void ForwardLeft() {
  analogWrite(ENA, SPEED);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, SPEED / speed_Coeff);
}

void BackwardLeft() {
  analogWrite(ENA, SPEED);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENB, SPEED / speed_Coeff);
}

void ForwardRight() {
  analogWrite(ENA, SPEED / speed_Coeff);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, SPEED);
}

void BackwardRight() {
  analogWrite(ENA, SPEED / speed_Coeff);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENB, SPEED);
}

void Stop() {
  analogWrite(ENA, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, 0);
}