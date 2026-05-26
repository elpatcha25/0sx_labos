#include "Convoyeur.h"
#include <Arduino.h>

const int VITESSE_DEPART  = 0;
const int INCREMENT       = 1;
const int VITESSE_MINIMUM = 80;
const int VITESSE_MAXIMUM = 255;
const int CENTRE_JOYSTICK = 512;

Convoyeur::Convoyeur(int brancheEntree1, int brancheEntree2,
                     int brancheActivation,
                     int brancheLedActive, int brancheLedUrgence)
  : _brancheEntree1(brancheEntree1),
    _brancheEntree2(brancheEntree2),
    _brancheActivation(brancheActivation),
    _brancheLedActive(brancheLedActive),
    _brancheLedUrgence(brancheLedUrgence),
    _enMarche(false), _enAvant(true), _vitesse(VITESSE_DEPART)
{}

void Convoyeur::debut() {
  pinMode(_brancheEntree1,    OUTPUT);
  pinMode(_brancheEntree2,    OUTPUT);
  pinMode(_brancheActivation, OUTPUT);
  pinMode(_brancheLedActive,  OUTPUT);
  pinMode(_brancheLedUrgence, OUTPUT);

  stopper();
  allumerLedUrgence(false);
}

void Convoyeur::modifierVitesse(int valeurX) {
  int pas = map(abs(valeurX), 0, CENTRE_JOYSTICK, 5, 40);
  pas = constrain(pas, 5, 40);

  if (valeurX > 0) {
    _vitesse = constrain(_vitesse + pas, VITESSE_MINIMUM, VITESSE_MAXIMUM);
  } else {
    _vitesse = constrain(_vitesse - pas, VITESSE_MINIMUM, VITESSE_MAXIMUM);
  }

  Serial.print("VITESSE: "); Serial.println(_vitesse);
  _envoyerVitesse();
}

void Convoyeur::_envoyerVitesse() {
  if (_enMarche) {
    analogWrite(_brancheActivation, _vitesse);
  }
}

void Convoyeur::allerAvant() {
  _enMarche = true;
  _enAvant  = true;

  if (_vitesse < VITESSE_MINIMUM) _vitesse = VITESSE_MINIMUM;
  digitalWrite(_brancheEntree1, HIGH);
  digitalWrite(_brancheEntree2, LOW);
  analogWrite(_brancheActivation, _vitesse);

  _mettreAjourLed();
}

void Convoyeur::allerArriere() {
  _enMarche = true;
  _enAvant  = false;

  if (_vitesse == 0) _vitesse = VITESSE_MINIMUM;
  digitalWrite(_brancheEntree1, LOW);
  digitalWrite(_brancheEntree2, HIGH);
  analogWrite(_brancheActivation, _vitesse);

  _mettreAjourLed();
}

void Convoyeur::stopper() {
  _enMarche = false;

  analogWrite(_brancheActivation, 0);
  digitalWrite(_brancheEntree1, LOW);
  digitalWrite(_brancheEntree2, LOW);

  _mettreAjourLed();
}

bool Convoyeur::estEnMarche() const {
  return _enMarche;
}

bool Convoyeur::estEnAvant() const {
  return _enAvant;
}

int Convoyeur::lireVitesse() const {
  return _vitesse;
}

void Convoyeur::allumerLedUrgence(bool allumee) {
  digitalWrite(_brancheLedUrgence, allumee ? HIGH : LOW);
}

void Convoyeur::_mettreAjourLed() {
  digitalWrite(_brancheLedActive, _enMarche ? HIGH : LOW);
}
