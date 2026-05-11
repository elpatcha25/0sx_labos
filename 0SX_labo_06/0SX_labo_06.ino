

#include "GestionnaireAffiche.h"

GestionnaireAffiche affiche;

// Fonction d'interruption pour le bouton urgence
void ISR_boutonUrgence() {
    affiche.signalerUrgence();
}

void setup() {
    affiche.demarrer();
    pinMode(BROCHE_URGENCE, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BROCHE_URGENCE), ISR_boutonUrgence, FALLING);
}

void loop() {
    affiche.mettreAJour();
}
