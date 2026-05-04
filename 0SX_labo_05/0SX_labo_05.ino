//tematio tsakeng
#include <Wire.h>
#include <LCD_I2C.h>
#include "Convoyeur.h"

const int BROCHE_JOYSTICK_Y  = A0;
const int BROCHE_JOYSTICK_X  = A1;
const int BROCHE_BOUTON_URG  = 3;
const int BROCHE_MOTEUR_IN1  = 44;
const int BROCHE_MOTEUR_IN2  = 45;
const int BROCHE_MOTEUR_EN   = 6;
const int BROCHE_LED_ACTIVE  = 7;
const int BROCHE_LED_URGENCE = 8;

const int CENTRE_JOYSTICK     = 512;
const int ZONE_MORTE_JOYSTICK = 150;

const unsigned long DELAI_PAGE_LCD = 2000;
const unsigned long DEBOUNCE_URG   = 200;
const unsigned long DELAI_VITESSE  = 80;

LCD_I2C ecran(0x27, 16, 2);
Convoyeur convoyeur(BROCHE_MOTEUR_IN1, BROCHE_MOTEUR_IN2,
                    BROCHE_MOTEUR_EN,
                    BROCHE_LED_ACTIVE, BROCHE_LED_URGENCE);


enum EtatSysteme {
  ETAT_ARRET,
  ETAT_MARCHE,
  ETAT_URGENCE
};

EtatSysteme etatSysteme = ETAT_ARRET;

unsigned long dernierAjustVitesse = 0;
unsigned long dernierDefilement   = 0;
unsigned long dernierBoutonUrg    = 0;

int  pageLCD         = 0;
bool btnUrgPrecedent = HIGH;

void setup() {
  Serial.begin(9600);

  pinMode(BROCHE_BOUTON_URG, INPUT_PULLUP);

  convoyeur.debut();

  ecran.begin();
  ecran.backlight();
  ecran.clear();
  ecran.setCursor(0, 0);
  ecran.print("  Convoyeur   ");
  ecran.setCursor(0, 1);
  ecran.print("   Pret...      ");
  delay(1500);
  ecran.clear();
}

void loop() {
  unsigned long maintenant = millis();

  gererBoutonUrgence(maintenant);

  switch (etatSysteme) {
    case ETAT_ARRET:   traiterArret();             break;
    case ETAT_MARCHE:  traiterMarche(maintenant);  break;
    case ETAT_URGENCE: traiterUrgence();            break;
  }

  afficherEcran(maintenant);
}

void gererBoutonUrgence(unsigned long maintenant) {
  bool btnActuel = digitalRead(BROCHE_BOUTON_URG);

  if (btnUrgPrecedent == HIGH && btnActuel == LOW &&
      (maintenant - dernierBoutonUrg) > DEBOUNCE_URG) {
    dernierBoutonUrg = maintenant;

    if (etatSysteme != ETAT_URGENCE) {
      activerUrgence();
    } else {
      desactiverUrgence();
    }
  }

  btnUrgPrecedent = btnActuel;
}

void activerUrgence() {
  convoyeur.stopper();
  convoyeur.allumerLedUrgence(true);
  etatSysteme = ETAT_URGENCE;
  pageLCD     = 0;
  afficherUrgence();
}

void desactiverUrgence() {
  convoyeur.allumerLedUrgence(false);
  etatSysteme = ETAT_ARRET;
  pageLCD     = 0;
  ecran.clear();
}

void traiterArret() {
  int lectureY = analogRead(BROCHE_JOYSTICK_Y);
  int deltaY   = lectureY - CENTRE_JOYSTICK;

  if (abs(deltaY) > ZONE_MORTE_JOYSTICK) {
    etatSysteme = ETAT_MARCHE;
    appliquerDirection(deltaY);
  }
}

void traiterMarche(unsigned long maintenant) {
  int lectureY = analogRead(BROCHE_JOYSTICK_Y);
  int lectureX = analogRead(BROCHE_JOYSTICK_X);
  int deltaY   = lectureY - CENTRE_JOYSTICK;
  int deltaX   = lectureX - CENTRE_JOYSTICK;

  if (abs(deltaX) > ZONE_MORTE_JOYSTICK &&
      (maintenant - dernierAjustVitesse) >= DELAI_VITESSE) {
    dernierAjustVitesse = maintenant;
    convoyeur.modifierVitesse(deltaX);
  }

  if (abs(deltaY) <= ZONE_MORTE_JOYSTICK) {
    convoyeur.stopper();
    etatSysteme = ETAT_ARRET;
  } else {
    appliquerDirection(deltaY);
  }
}

void traiterUrgence() {
  // Tout est gelé
}

void appliquerDirection(int deltaY) {
  if (deltaY > 0) {
    convoyeur.allerAvant();
  } else {
    convoyeur.allerArriere();
  }
}

void afficherEcran(unsigned long maintenant) {
  if (etatSysteme == ETAT_URGENCE) return;

  if (maintenant - dernierDefilement >= DELAI_PAGE_LCD) {
    dernierDefilement = maintenant;
    pageLCD = (pageLCD + 1) % 2;
    ecran.clear();
  }

  afficherPageCourante();
}

void afficherPageCourante() {
  // Page 0 : convoyeur actif + sens
  if (pageLCD == 0) {
    ecran.setCursor(0, 0);
    ecran.print("Actif: ");
    ecran.print(convoyeur.estEnMarche() ? "OUI  " : "NON  ");

    ecran.setCursor(0, 1);
    ecran.print("Sens: ");
    if (!convoyeur.estEnMarche()) {
      ecran.print("------    ");
    } else {
      ecran.print(convoyeur.estEnAvant() ? "AVANT     " : "ARRIERE   ");
    }
  }

  // Page 1 : vitesse
  else if (pageLCD == 1) {
    ecran.setCursor(0, 0);
    ecran.print("Vitesse:        ");
    ecran.setCursor(0, 1);
    ecran.print(convoyeur.lireVitesse());
    ecran.print(" /255          ");
  }
}

void afficherUrgence() {
  ecran.clear();
  ecran.setCursor(3, 0);
  ecran.print("!! URGENCE !!");
  ecran.setCursor(2, 1);
  ecran.print("Appui = reset");
}
