#ifndef PANNEAU_MATRICE_H
#define PANNEAU_MATRICE_H

#include <Arduino.h>
#include <LedControl.h>
#include "EtatAffiche.h"

#define DELAI_DEFILEMENT  150   
#define DELAI_CLIGNOTEMENT 400  


//  Classe  de ma matrice LED

class PanneauMatrice {
public:
    PanneauMatrice(uint8_t brancheDin, uint8_t brancheClk, uint8_t brancheCs);
    void demarrer();
    void mettreAJour(EtatAffiche etat);
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

#endif
