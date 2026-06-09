//moteurs équilibré

#include <WiFi.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

// pins des moteurs
#define ENA 25
#define IN1 26
#define IN2 27
#define ENB 13
#define IN3 14
#define IN4 12

//pins capteurs
#define IR_GAUCHE 4
#define IR_CENTRE 15
#define IR_DROITE 2

// vitesse
#define VITESSE_NORMALE 150
#define VITESSE_GAUCHE 180     // Plus puissant pour équilibrer
#define VITESSE_TOURNANT 200

const char* ssid = "VoitureESP32";
const char* password = "12345678";

WebSocketsServer ws(81);

String etat = "STOP";
unsigned long startTime = 0;
float distanceCm = 0;
unsigned long lastSend = 0;

void avancer() {
  analogWrite(ENA, VITESSE_NORMALE);
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  analogWrite(ENB, VITESSE_GAUCHE);  // Moteur gauche plus puissant
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  etat = "AVANCER";
}

void tournerGauche() {
  analogWrite(ENA, VITESSE_TOURNANT);
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  analogWrite(ENB, 0);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
  etat = "TOURNER_GAUCHE";
  delay(350);
}

void tournerDroite() {
  analogWrite(ENA, 0);
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  analogWrite(ENB, VITESSE_TOURNANT);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  etat = "TOURNER_DROITE";
  delay(350);
}

// Detection de la ligne

bool voitLigneGauche() {
  return digitalRead(IR_GAUCHE) == 0;
}

bool voitLigneCentre() {
  return digitalRead(IR_CENTRE) == 1;
}

bool voitLigneDroite() {
  return digitalRead(IR_DROITE) == 0;
}

void suivreLigne() {
  bool g = voitLigneGauche();
  bool c = voitLigneCentre();
  bool d = voitLigneDroite();
  
  static unsigned long lastDebug = 0;
  if (millis() - lastDebug > 200) {
    lastDebug = millis();
    Serial.print("G="); Serial.print(digitalRead(IR_GAUCHE));
    Serial.print(" C="); Serial.print(digitalRead(IR_CENTRE));
    Serial.print(" D="); Serial.print(digitalRead(IR_DROITE));
    Serial.print(" → ");
    Serial.println(etat);
  }
  
  if (c) {
    avancer();
  }
  else if (g) {
    tournerGauche();
  }
  else if (d) {
    tournerDroite();
  }
  else {
    avancer();
  }
}

void sendData() {
  if (millis() - lastSend < 200) return;
  lastSend = millis();
  
  bool g = voitLigneGauche();
  bool c = voitLigneCentre();
  bool d = voitLigneDroite();
  
  int speedRight = 0, speedLeft = 0;
  if (etat == "AVANCER")            { speedRight = VITESSE_NORMALE; speedLeft = VITESSE_GAUCHE; }
  else if (etat == "TOURNER_GAUCHE"){ speedRight = VITESSE_TOURNANT; speedLeft = 0; }
  else if (etat == "TOURNER_DROITE"){ speedRight = 0; speedLeft = VITESSE_TOURNANT; }
  
  StaticJsonDocument<256> doc;
  doc["state"] = etat;
  doc["speedRight"] = speedRight;
  doc["speedLeft"] = speedLeft;
  doc["sensorLeft"] = digitalRead(IR_GAUCHE);
  doc["sensorCenter"] = digitalRead(IR_CENTRE);
  doc["sensorRight"] = digitalRead(IR_DROITE);
  doc["activeLeft"] = g;
  doc["activeCenter"] = c;
  doc["activeRight"] = d;
  doc["distance"] = (int)distanceCm;
  doc["uptime"] = (millis() - startTime) / 1000;
  doc["freeHeap"] = ESP.getFreeHeap() / 1024;
  
  String json;
  serializeJson(doc, json);
  ws.broadcastTXT(json);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  pinMode(ENA, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  pinMode(IR_GAUCHE, INPUT);
  pinMode(IR_CENTRE, INPUT);
  pinMode(IR_DROITE, INPUT);
  
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  
  Serial.println("\nSuiveur de ligne \n");
  
  // Test moteurs
  Serial.println("Test moteur DROIT...");
  analogWrite(ENA, 200);
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  delay(2000);
  analogWrite(ENA, 0);
  
  Serial.println("Test moteur GAUCHE...");
  analogWrite(ENB, 200);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  delay(2000);
  analogWrite(ENB, 0);
  
  WiFi.softAP(ssid, password);
  ws.begin();
  startTime = millis();
  
  Serial.println("\nValeurs capteurs:");
  Serial.print("GAUCHE: "); Serial.println(digitalRead(IR_GAUCHE));
  Serial.print("CENTRE: "); Serial.println(digitalRead(IR_CENTRE));
  Serial.print("DROITE: "); Serial.println(digitalRead(IR_DROITE));
  
  Serial.println("\nPose la voiture sur la LIGNE NOIRE");
  Serial.println("Demarrage dans 5 secondes...");
  delay(5000);
  Serial.println("GO !\n");
}

void loop() {
  suivreLigne();
  sendData();
  ws.loop();
  delay(30);
}