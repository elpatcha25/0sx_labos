#pragma once

#include <Arduino.h>
#include <Servo.h>
#include "Etats.h"

const int ANGLE_FERME               = 10;
const int ANGLE_OUVERT              = 170;
const int SEUIL_DISTANCE_PORTE      = 20;
const unsigned long DELAI_OUVERTURE = 10000;
const unsigned long DELAI_SERVO     = 15;

class PorteAutomatique {
public:
    PorteAutomatique(int broche);
    void demarrer();
    void mettreAJour(unsigned long maintenant, float distanceCm);
    void ouvrirManuellement();
    void activerUrgence();
    void desactiverUrgence();

    int         lireAngle()     const;
    EtatPorte   lireEtat()      const;
    const char* lireEtatTexte() const;

private:
    Servo         _servo;
    int           _broche;
    EtatPorte     _etat;
    int           _angle;
    unsigned long _tempsOuverture;
    unsigned long _dernierServo;

    void _gererEtat(unsigned long maintenant, float distanceCm);
    void _deplacerServo(unsigned long maintenant);
};
