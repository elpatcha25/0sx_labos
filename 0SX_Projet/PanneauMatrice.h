#pragma once

#include <Arduino.h>
#include <LedControl.h>
#include "Etats.h"

#define DELAI_DEFILEMENT  150   // ms entre chaque colonne défilée
#define DELAI_CLIGNOTEMENT 400  // ms entre chaque clignotement (mode erreur)

class PanneauMatrice {
public:
    PanneauMatrice(uint8_t brancheDin, uint8_t brancheClk, uint8_t brancheCs);
    void demarrer();
    void mettreAJour(ModeAffiche etat);
    void afficherImage(const uint8_t image[8]);
    void effacer();

private:
    LedControl    _panneau;
    int           _positionDefilement;
    unsigned long _dernierDefilement;
    bool          _clignotementActif;
    unsigned long _dernierClignotement;

    void _afficherSmiley();
    void _defilerSpecial();
    void _clignoterErreur();
    void _afficherCroix();
};

