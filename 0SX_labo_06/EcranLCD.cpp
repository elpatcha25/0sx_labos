#include "EcranLCD.h"

// Noms affichés sur le LCD pour chaque mode
static const String NOMS_MODE[] = {
    "Normal",
    "Rabais",
    "Erreur",
    "Ferme",
    "ARRET URGENCE"
};

EcranLCD::EcranLCD(uint8_t adresse, uint8_t colonnes, uint8_t rangees)
    : _ecran(adresse, colonnes, rangees),
      _dernierEtat(EtatAffiche::NORMAL)
{}

void EcranLCD::demarrer() {
    _ecran.init();
    _ecran.backlight();
    _ecran.clear();
    _ecran.setCursor(0, 0);
    _ecran.print(F("Mode:"));
    _ecran.setCursor(0, 1);
    _ecran.print(F("Normal"));
}

void EcranLCD::afficherMode(EtatAffiche etat) {
    if (etat == _dernierEtat) return;  // rien à changer
    _dernierEtat = etat;
    _ecran.clear();
    _ecran.setCursor(0, 0);
    _ecran.print(F("Mode:"));
    _ecran.setCursor(0, 1);
    _ecran.print(NOMS_MODE[(int)etat]);
}
