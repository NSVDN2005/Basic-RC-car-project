#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>  // SPIFFS support

// Motor control pins
int enA = D1;
int in1 = D2;
int in2 = D3;
int in3 = D4;
int in4 = D5;
int enB = D6;

const int buzPin = D7;
const int wifiLedPin = D0;

ESP8266WebServer server(80);

String command;
int SPEED = 1023;
int speed_Coeff = 3;

void setup() {
  Serial.begin(115200);

  pinMode(buzPin, OUTPUT);
  pinMode(wifiLedPin, OUTPUT);

  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(enB, OUTPUT);

  analogWrite(enA, SPEED);
  analogWrite(enB, SPEED);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);

  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS Mount Failed");
    return;
  }

  WiFi.softAP("ESP-CAR", "12345678");
  digitalWrite(wifiLedPin, LOW);
  Serial.println("AP Started: " + WiFi.softAPIP().toString());

  server.on("/", handleRoot);

  // Add ping route
  server.on("/ping", []() {
    server.send(200, "text/plain", "pong");
  });

  server.begin();
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  if (server.hasArg("speed")) {
    SPEED = server.arg("speed").toInt();
  }

  command = server.arg("State");
  if (command != "") {
    Serial.println("Command: " + command);
    updateControl(command);
    server.send(200, "text/plain", "speed=" + String(SPEED) + ";temp=25;hum=60;servo=90;");
    return;
  }

  File file = SPIFFS.open("/rc.html", "r");
  if (file) {
    server.streamFile(file, "text/html");
    file.close();
  } else {
    server.send(500, "text/plain", "Error loading interface");
  }
}

void updateControl(String cmd) {
  if (cmd == "F") Forward();
  else if (cmd == "B") Backward();
  else if (cmd == "L") TurnLeft();
  else if (cmd == "R") TurnRight();
  else if (cmd == "G") ForwardLeft();
  else if (cmd == "H") BackwardLeft();
  else if (cmd == "I") ForwardRight();
  else if (cmd == "J") BackwardRight();
  else if (cmd == "S") Stop();
  else if (cmd == "V") BeepHorn();
}

void Forward() {
  analogWrite(enA, SPEED);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  analogWrite(enB, SPEED);
}

void Backward() {
  analogWrite(enA, SPEED);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite(enB, SPEED);
}

void TurnRight() {
  analogWrite(enA, SPEED);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite(enB, SPEED);
}

void TurnLeft() {
  analogWrite(enA, SPEED);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  analogWrite(enB, SPEED);
}

void ForwardLeft() {
  analogWrite(enA, SPEED);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  analogWrite(enB, SPEED / speed_Coeff);
}

void BackwardLeft() {
  analogWrite(enA, SPEED);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite(enB, SPEED / speed_Coeff);
}

void ForwardRight() {
  analogWrite(enA, SPEED / speed_Coeff);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  analogWrite(enB, SPEED);
}

void BackwardRight() {
  analogWrite(enA, SPEED / speed_Coeff);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite(enB, SPEED);
}

void Stop() {
  analogWrite(enA, 0);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  analogWrite(enB, 0);
}

void BeepHorn() {
  digitalWrite(buzPin, HIGH);
  delay(150);
  digitalWrite(buzPin, LOW);
  delay(80);
}
