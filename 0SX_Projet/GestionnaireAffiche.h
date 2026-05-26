#pragma once

#include <Arduino.h>
#include "Etats.h"
#include "EcranLCD.h"
#include "PanneauMatrice.h"

constexpr uint8_t BTN_IR_RABAIS = 0x0C;
constexpr uint8_t BTN_IR_NORMAL = 0x18;
constexpr uint8_t BTN_IR_ERREUR = 0x5E;
constexpr uint8_t BTN_IR_FERMER = 0x08;

class GestionnaireAffiche {
public:
    GestionnaireAffiche(int brocheDIN, int brocheCLK, int brocheCS);
    void demarrer();
    void mettreAJour();
    void signalerUrgence();
    void changerMode(ModeAffiche nouveauMode);
    ModeAffiche obtenirMode() const;

private:
    ModeAffiche   _modeActuel;
    ModeAffiche   _modeAvantUrgence;
    volatile bool _urgenceSignalee;

    EcranLCD       _ecran;
    PanneauMatrice _panneau;

    void _lireCommandeSerie();
    void _traiterUrgence();
};
