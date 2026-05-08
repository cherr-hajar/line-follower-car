
// Moteur DROIT (câbles inversés physiquement)
#define ENA 25
#define IN1 26
#define IN2 27

// Moteur GAUCHE
#define ENB 13
#define IN3 14
#define IN4 12

//  PINS CAPTEURS IR
#define IR_GAUCHE 33
#define IR_CENTRE 35
#define IR_DROITE 34

//  VITESSES OPTIMISEES
#define VITESSE_NORMALE 140      // Vitesse normale réduite pour mieux contrôler
#define VITESSE_TOURNANT 180     // Virage plus rapide
#define VITESSE_CORRECTION 90    // Correction légère

String etat = "STOP";
int seuilGauche = 0, seuilCentre = 0, seuilDroite = 0;

// FONCTIONS MOTEUR

void avancer() {
  analogWrite(ENA, VITESSE_NORMALE);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  
  analogWrite(ENB, VITESSE_NORMALE);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  
  etat = "AVANCER";
}

void correctionGauche() {
  // Tourne légèrement à gauche
  analogWrite(ENA, VITESSE_NORMALE);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  
  analogWrite(ENB, VITESSE_CORRECTION);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  
  etat = "← CORRECTION GAUCHE";
}

void correctionDroite() {
  // Tourne légèrement à droite
  analogWrite(ENA, VITESSE_CORRECTION);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  
  analogWrite(ENB, VITESSE_NORMALE);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  
  etat = "CORRECTION DROITE →";
}

void tournerGauche() {
  // Virage serré à gauche: moteur droit avance, moteur gauche recule
  analogWrite(ENA, VITESSE_TOURNANT);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  
  analogWrite(ENB, VITESSE_TOURNANT);
  digitalWrite(IN3, LOW);      // Moteur gauche en arrière
  digitalWrite(IN4, HIGH);
  
  etat = "←← TOURNER GAUCHE (SERRE)";
}

void tournerDroite() {
  // Virage serré à droite: moteur gauche avance, moteur droit recule
  analogWrite(ENA, VITESSE_TOURNANT);
  digitalWrite(IN1, LOW);      // Moteur droit en arrière
  digitalWrite(IN2, HIGH);
  
  analogWrite(ENB, VITESSE_TOURNANT);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  
  etat = "TOURNER DROITE (SERRE) →→";
}

void stopper() {
  analogWrite(ENA, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  
  analogWrite(ENB, 0);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  
  etat = "STOP";
}

// CALIBRATION

void calibrer() {
  Serial.println("\n=== CALIBRATION ===");
  
  // 1. Lecture SOL CLAIR
  Serial.println("1. POSEZ SUR BLANC - Attendez 3 secondes...");
  delay(3000);
  
  int blancG = 0, blancC = 0, blancD = 0;
  for (int i = 0; i < 50; i++) {
    blancG += analogRead(IR_GAUCHE);
    blancC += analogRead(IR_CENTRE);
    blancD += analogRead(IR_DROITE);
    delay(10);
  }
  blancG /= 50; blancC /= 50; blancD /= 50;
  
  Serial.print("BLANC → G:"); Serial.print(blancG);
  Serial.print(" C:"); Serial.print(blancC);
  Serial.print(" D:"); Serial.println(blancD);
  
  // 2. Lecture LIGNE NOIRE
  Serial.println("2. POSEZ SUR NOIR - Attendez 3 secondes...");
  delay(3000);
  
  int noirG = 0, noirC = 0, noirD = 0;
  for (int i = 0; i < 50; i++) {
    noirG += analogRead(IR_GAUCHE);
    noirC += analogRead(IR_CENTRE);
    noirD += analogRead(IR_DROITE);
    delay(10);
  }
  noirG /= 50; noirC /= 50; noirD /= 50;
  
  Serial.print("NOIR → G:"); Serial.print(noirG);
  Serial.print(" C:"); Serial.print(noirC);
  Serial.print(" D:"); Serial.println(noirD);
  
  // Calcul des seuils
  seuilGauche = (blancG + noirG) / 2;
  seuilCentre = (blancC + noirC) / 2;
  seuilDroite = (blancD + noirD) / 2;
  
  Serial.println("\n=== SEUILS ===");
  Serial.print("G:"); Serial.println(seuilGauche);
  Serial.print("C:"); Serial.println(seuilCentre);
  Serial.print("D:"); Serial.println(seuilDroite);
  
  // Vérification
  if (noirG > blancG) {
    Serial.println("⚠️ LIGNE BLANCHE sur fond NOIR - Inversion auto");
  }
  
  Serial.println("✅ CALIBRATION OK\n");
  delay(1000);
}

bool surLigne(int pin, int seuil) {
  return analogRead(pin) < seuil;
}

// SUIVI DE LIGNE

void suivreLigne() {
  bool g = surLigne(IR_GAUCHE, seuilGauche);
  bool c = surLigne(IR_CENTRE, seuilCentre);
  bool d = surLigne(IR_DROITE, seuilDroite);
  
  // AFFICHAGE DEBUG
  static unsigned long lastDebug = 0;
  if (millis() - lastDebug > 200) {
    lastDebug = millis();
    
    Serial.print("[");
    Serial.print(g ? "⬛" : "⬜");
    Serial.print("][");
    Serial.print(c ? "⬛" : "⬜");
    Serial.print("][");
    Serial.print(d ? "⬛" : "⬜");
    Serial.print("] ");
    Serial.print("G:"); Serial.print(analogRead(IR_GAUCHE));
    Serial.print(" C:"); Serial.print(analogRead(IR_CENTRE));
    Serial.print(" D:"); Serial.print(analogRead(IR_DROITE));
    Serial.print(" → ");
  }
  
  // LOGIQUE DE DÉCISION
  if (c && !g && !d) {
    avancer();
    if (millis() - lastDebug < 200) Serial.println("AVANCER");
  }
  else if (g && !c && !d) {
    correctionGauche();
    if (millis() - lastDebug < 200) Serial.println("← CORRECTION GAUCHE");
  }
  else if (d && !c && !g) {
    correctionDroite();
    if (millis() - lastDebug < 200) Serial.println("CORRECTION DROITE →");
  }
  else if (g && c && !d) {
    tournerGauche();
    if (millis() - lastDebug < 200) Serial.println("←← TOURNER GAUCHE");
  }
  else if (d && c && !g) {
    tournerDroite();
    if (millis() - lastDebug < 200) Serial.println("TOURNER DROITE →→");
  }
  else if (g && d && !c) {
    avancer();
    if (millis() - lastDebug < 200) Serial.println("AVANCER (FOURCHE)");
  }
  else if (g && c && d) {
    avancer();
    if (millis() - lastDebug < 200) Serial.println("AVANCER (LARGE)");
  }
  else {
    // Ligne perdue - cherche en tournant sur place
    // quand ca sort du truc 
    static unsigned long lastTurn = 0;
    static bool turnLeft = true;
    if (millis() - lastTurn > 400) {
      if (turnLeft) {
        tournerGauche();
      } else {
        tournerDroite();
      }
      turnLeft = !turnLeft;
      lastTurn = millis();
    }
    if (millis() - lastDebug < 200) Serial.println("🔍 RECHERCHE LIGNE");
  }
}

// SETUP
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Configuration
  pinMode(ENA, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  pinMode(IR_GAUCHE, INPUT); pinMode(IR_CENTRE, INPUT); pinMode(IR_DROITE, INPUT);
  
  stopper();
  
  Serial.println("\n╔════════════════════════════════╗");
  Serial.println("║    SUIVEUR DE LIGNE 3 VOIES    ║");
  Serial.println("║       VERSION FINALE           ║");
  Serial.println("╚════════════════════════════════╝\n");
  
  // Test moteurs rapide
  Serial.println("Test moteurs (2 secondes)...");
  avancer();
  delay(2000);
  stopper();
  
  calibrer();
  
  Serial.println("\n▶ DÉMARRAGE DANS 3 SECONDES...");
  for (int i = 3; i > 0; i--) {
    Serial.print(i);
    Serial.print("... ");
    delay(1000);
  }
  Serial.println("GO ! 🚗\n");
}

void loop() {
  suivreLigne();
  delay(30);
}