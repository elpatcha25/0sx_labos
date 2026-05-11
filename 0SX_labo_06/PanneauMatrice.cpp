#include "PanneauMatrice.h"


static const uint8_t IMAGE_SMILEY[8] = {
    0b00111100, 0b01000010, 0b10100101, 0b10000001,
    0b10100101, 0b10011001, 0b01000010, 0b00111100
};

static const uint8_t IMAGE_CROIX[8] = {
    0b10000001, 0b01000010, 0b00100100, 0b00011000,
    0b00011000, 0b00100100, 0b01000010, 0b10000001
};

// mot "SPECIAL" 
static const uint8_t POLICE_SPECIAL[][5] = {
    { 0x62, 0x91, 0x91, 0x91, 0x4E },  // S
    { 0xFF, 0x88, 0x88, 0x88, 0x70 },  // P
    { 0xFF, 0x89, 0x89, 0x89, 0x81 },  // E
    { 0x7E, 0x81, 0x81, 0x81, 0x42 },  // C
    { 0x00, 0x81, 0xFF, 0x81, 0x00 },  // I
    { 0x7F, 0x88, 0x88, 0x88, 0x7F },  // A
    { 0xFF, 0x01, 0x01, 0x01, 0x01 },  // L
    { 0x00, 0x00, 0x00, 0x00, 0x00 }   // espace
};

static const int NB_CARACTERES   = 8;
static const int LARGEUR_COLONNE = 6;


//  Constructeur

PanneauMatrice::PanneauMatrice(uint8_t brancheDin, uint8_t brancheClk, uint8_t brancheCs)
    : _panneau(brancheDin, brancheClk, brancheCs, 1),
      _positionDefilement(0),
      _dernierDefilement(0),
      _clignotementActif(false),
      _dernierClignotement(0)
{}

//  Initialisation

void PanneauMatrice::demarrer() {
    _panneau.shutdown(0, false);
    _panneau.setIntensity(0, 5);
    _panneau.clearDisplay(0);
}


//  Mise à jour selon l'état

void PanneauMatrice::mettreAJour(EtatAffiche etat) {
    switch (etat) {
        case EtatAffiche::NORMAL:  _afficherSmiley();   break;
        case EtatAffiche::RABAIS:  _defilerSpecial();   break;
        case EtatAffiche::ERREUR:  _clignoterErreur();  break;
        case EtatAffiche::FERMER:                       break;  // tout éteint
        case EtatAffiche::URGENCE: _afficherCroix();    break;
    }
}


void PanneauMatrice::afficherImage(const uint8_t image[8]) {
    for (int rangee = 0; rangee < 8; rangee++) {
        _panneau.setRow(0, rangee, image[rangee]);
    }
}


void PanneauMatrice::effacer() {
    _panneau.clearDisplay(0);
}


void PanneauMatrice::_afficherSmiley() {
    afficherImage(IMAGE_SMILEY);
}

void PanneauMatrice::_afficherCroix() {
    afficherImage(IMAGE_CROIX);
}

void PanneauMatrice::_defilerSpecial() {
    unsigned long maintenant = millis();
    if (maintenant - _dernierDefilement < DELAI_DEFILEMENT) return;
    _dernierDefilement = maintenant;

    for (int colonne = 0; colonne < 8; colonne++) {
        int source    = (_positionDefilement + colonne) % (NB_CARACTERES * LARGEUR_COLONNE);
        int idxChar   = source / LARGEUR_COLONNE;
        int idxColone = source % LARGEUR_COLONNE;
        int donnee    = (idxColone < 5) ? POLICE_SPECIAL[idxChar][idxColone] : 0x00;
        _panneau.setColumn(0, colonne, donnee);
    }
    _positionDefilement = (_positionDefilement + 1) % (NB_CARACTERES * LARGEUR_COLONNE);
}

void PanneauMatrice::_clignoterErreur() {
    unsigned long maintenant = millis();
    if (maintenant - _dernierClignotement < DELAI_CLIGNOTEMENT) return;
    _dernierClignotement = maintenant;
    _clignotementActif   = !_clignotementActif;

    for (int rangee = 0; rangee < 8; rangee++) {
        _panneau.setRow(0, rangee, _clignotementActif ? 0xFF : 0x00);
    }
}
