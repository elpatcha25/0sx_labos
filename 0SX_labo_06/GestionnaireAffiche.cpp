#include "GestionnaireAffiche.h"
#include <IRremote.hpp>

GestionnaireAffiche::GestionnaireAffiche()
    : _modeActuel(EtatAffiche::NORMAL),
      _modeAvantUrgence(EtatAffiche::NORMAL),
      _urgenceSignalee(false),
      _ecran(ADRESSE_LCD, 16, 2),
      _panneau(BROCHE_DIN, BROCHE_CLK, BROCHE_CS)
{}

//  Initialisation

void GestionnaireAffiche::demarrer() {
    _ecran.demarrer();
    _panneau.demarrer();
    IrReceiver.begin(BROCHE_IR, DISABLE_LED_FEEDBACK);
    Serial.begin(9600);
    Serial.println(F("Commandes serie: 1=Rabais 2=Normal 3=Erreur 4=Ferme"));
}


void GestionnaireAffiche::mettreAJour() {
    if (_urgenceSignalee) {
        _urgenceSignalee = false;
        _traiterUrgence();
        return;
    }

    if (_modeActuel == EtatAffiche::URGENCE) return;

    _lireCommandeIR();
    _lireCommandeSerie();
    _panneau.mettreAJour(_modeActuel);
}

//  Changer de mode

void GestionnaireAffiche::changerMode(EtatAffiche nouveauMode) {
    if (nouveauMode == _modeActuel) return;
    _modeActuel = nouveauMode;
    _reinitialiserPosition();
    _panneau.effacer();
    _ecran.afficherMode(_modeActuel);
}

void GestionnaireAffiche::signalerUrgence() {
    _urgenceSignalee = true;
}


void GestionnaireAffiche::_lireCommandeIR() {
    if (!IrReceiver.decode()) return;

    if (IrReceiver.decodedIRData.decodedRawData != 0) {
        uint8_t commande = IrReceiver.decodedIRData.command;
        Serial.print(F("[IR] 0x")); Serial.println(commande, HEX);

        switch (commande) {
            case BTN_IR_RABAIS: changerMode(EtatAffiche::RABAIS); break;
            case BTN_IR_NORMAL: changerMode(EtatAffiche::NORMAL); break;
            case BTN_IR_ERREUR: changerMode(EtatAffiche::ERREUR); break;
            case BTN_IR_FERMER: changerMode(EtatAffiche::FERMER); break;
        }
    }
    IrReceiver.resume();
}

void GestionnaireAffiche::_lireCommandeSerie() {
    if (!Serial.available()) return;

    switch (Serial.read()) {
        case '1': changerMode(EtatAffiche::RABAIS); break;
        case '2': changerMode(EtatAffiche::NORMAL); break;
        case '3': changerMode(EtatAffiche::ERREUR); break;
        case '4': changerMode(EtatAffiche::FERMER); break;
        default:
            Serial.println(F("Commande invalide. 1=Rabais 2=Normal 3=Erreur 4=Ferme"));
            break;
    }
}

void GestionnaireAffiche::_traiterUrgence() {
    if (_modeActuel != EtatAffiche::URGENCE) {
        _modeAvantUrgence = _modeActuel;
        _modeActuel       = EtatAffiche::URGENCE;
        _panneau.mettreAJour(EtatAffiche::URGENCE);
        _ecran.afficherMode(EtatAffiche::URGENCE);
        Serial.println(F("Mode arret d'urgence active."));
    } else {
        _panneau.effacer();
        changerMode(_modeAvantUrgence);
        Serial.println(F("Systeme redémarre."));
    }
}


void GestionnaireAffiche::_reinitialiserPosition() {
    
}
