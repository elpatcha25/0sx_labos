#include <Servo.h>
#include <HCSR04.h>
#include <LCD_I2C.h>

#define TRIGGER_PIN 12
#define ECHO_PIN 11
#define SERVO_PIN 9
#define BTN_OPEN 2
#define BTN_URGENCE 3

HCSR04 hc(TRIGGER_PIN, ECHO_PIN);
LCD_I2C lcd(0x27, 16, 2);
Servo myServo;

const char* DA = "2407822";

unsigned long currentTime = 0;
float distance = 0.0;
int degree = 10;

const int SERVO_FERME   = 10;
const int SERVO_OUVERT  = 170;
const int DISTANCE_SEUIL = 20;

const int DISPLAY_INTERVAL   = 100;
const int DISTANCE_INTERVAL  = 50;
const int WAIT_OPEN_INTERVAL = 10000;
const int SERVO_STEP_INTERVAL = 15;

enum Etat {
  FERMEE,
  OUVERTURE,
  OUVERTE,
  FERMETURE,
  URGENCE
} currentState = FERMEE;

unsigned long openWaitTime  = 0;
unsigned long lastServoStep = 0;
int savedDegree             = SERVO_FERME;

bool lastBtnOpen    = HIGH;
bool lastBtnUrgence = HIGH;

void setup() {
  Serial.begin(115200);
  lcd.begin();
  lcd.backlight();

  pinMode(BTN_OPEN, INPUT_PULLUP);
  pinMode(BTN_URGENCE, INPUT_PULLUP);

  myServo.attach(SERVO_PIN);
  myServo.write(SERVO_FERME);
  degree = SERVO_FERME;

}

void loop() {
  currentTime = millis();

  static unsigned long previousDistance = 0;
  if (currentTime - previousDistance >= DISTANCE_INTERVAL) {
    distance = hc.dist();
    previousDistance = currentTime;
  }

  readButtons();
  stateManager();
  servoTask();
  screenDisplay(currentTime);
}

//  Lecture des boutons 

void readButtons() {
  bool btnOpenNow    = digitalRead(BTN_OPEN);
  bool btnUrgenceNow = digitalRead(BTN_URGENCE);

  // Bouton urgence
  if (lastBtnUrgence == HIGH && btnUrgenceNow == LOW) {
    if (currentState == URGENCE) {
      //  retour à FERMEE
      degree = SERVO_FERME;
      myServo.write(degree);
      currentState = FERMEE;
    } else {
      // Arrêt d'urgence
      savedDegree  = degree;
      currentState = URGENCE;
    }
  }

 
  if (lastBtnOpen == HIGH && btnOpenNow == LOW) {
    if (currentState == FERMEE) {
      currentState = OUVERTURE;
    } else if (currentState == FERMETURE) {
      currentState = OUVERTURE;
      degree = degree + 1; // force mon servo a ne pas etre bloquer a SERVO_FERME
    }
    // Si l' OUVERTURE est déjà en cours  j'ignore
  }

  lastBtnOpen    = btnOpenNow;
  lastBtnUrgence = btnUrgenceNow;
}

// états 

void stateManager() {
  switch (currentState) {
    case FERMEE:
      if (distance > 0 && distance < DISTANCE_SEUIL) {
        currentState = OUVERTURE;
      }
      break;

    case OUVERTURE:
      if (degree >= SERVO_OUVERT) {
        currentState = OUVERTE;
        openWaitTime = currentTime;
      }
      break;

    case OUVERTE:
      if (currentTime - openWaitTime >= WAIT_OPEN_INTERVAL) {
        currentState = FERMETURE;
      }
      break;

    case FERMETURE:
      if (degree <= SERVO_FERME) {
        currentState = FERMEE;
        return; 
      }
      break;

    case URGENCE:
 
      break;
  }
}


void servoTask() {
  if (currentState == URGENCE) return;

  if (currentTime - lastServoStep >= SERVO_STEP_INTERVAL) {
    if (currentState == OUVERTURE && degree < SERVO_OUVERT) {
      degree++;
      myServo.write(degree);
    } else if (currentState == FERMETURE && degree > SERVO_FERME) {
      degree--;
      myServo.write(degree);
    }
    lastServoStep = currentTime;
  }
}



// Affichage LCD 

void screenDisplay(unsigned long ct) {
  static unsigned long previousDisplay = 0;
  if (ct - previousDisplay >= DISPLAY_INTERVAL) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Dist: ");
    lcd.print((int)distance);
    lcd.print(" cm");

    lcd.setCursor(0, 1);
    switch (currentState) {
      case FERMEE:
        lcd.print("Porte: Fermee");
        break;
      case OUVERTE:
        lcd.print("Porte: Ouverte");
        break;
      case OUVERTURE:
        lcd.print("Ouverture...");
        break;
      case FERMETURE:
        lcd.print("Fermeture...");
        break;
      case URGENCE:
        lcd.print("!! URGENCE !!");
        break;
    }
    previousDisplay = ct;
  }
}
