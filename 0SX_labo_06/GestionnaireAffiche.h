#ifndef GESTIONNAIRE_AFFICHE_H
#define GESTIONNAIRE_AFFICHE_H

#include <Arduino.h>
#include "EtatAffiche.h"
#include "EcranLCD.h"
#include "PanneauMatrice.h"

//  Broches

#define BROCHE_IR       8
#define BROCHE_URGENCE  3
#define BROCHE_DIN      34
#define BROCHE_CLK      30
#define BROCHE_CS       32
#define ADRESSE_LCD     0x27

//  boutons télécommande IR

constexpr uint8_t BTN_IR_RABAIS = 0x0C;
constexpr uint8_t BTN_IR_NORMAL = 0x18;
constexpr uint8_t BTN_IR_ERREUR = 0x5E;
constexpr uint8_t BTN_IR_FERMER = 0x08;

//  Classe responsable de la coordination générale

class GestionnaireAffiche {
public:
    GestionnaireAffiche();
    void demarrer();
    void mettreAJour();
    void signalerUrgence();
    void changerMode(EtatAffiche nouveauMode);

private:
    EtatAffiche   _modeActuel;
    EtatAffiche   _modeAvantUrgence;
    volatile bool _urgenceSignalee;

    EcranLCD       _ecran;
    PanneauMatrice _panneau;

    void _lireCommandeIR();
    void _lireCommandeSerie();
    void _traiterUrgence();
    void _reinitialiserPosition();
};

#endif
