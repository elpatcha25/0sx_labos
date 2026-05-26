#include "GestionnaireAffiche.h"

GestionnaireAffiche::GestionnaireAffiche(int brocheDIN, int brocheCLK, int brocheCS)
    : _modeActuel(MODE_NORMAL),
      _modeAvantUrgence(MODE_NORMAL),
      _urgenceSignalee(false),
      _ecran(0x27, 16, 2),
      _panneau(brocheDIN, brocheCLK, brocheCS)
{}

void GestionnaireAffiche::demarrer() {
    _ecran.demarrer();
    _panneau.demarrer();
    Serial.println(F("Commandes: 1=Rabais 2=Normal 3=Erreur 4=Ferme"));
}

void GestionnaireAffiche::mettreAJour() {
    if (_urgenceSignalee) {
        _urgenceSignalee = false;
        _traiterUrgence();
        return;
    }
    if (_modeActuel == MODE_URGENCE) return;

    _lireCommandeSerie();
    _panneau.mettreAJour(_modeActuel);
}

void GestionnaireAffiche::changerMode(ModeAffiche nouveauMode) {
    if (nouveauMode == _modeActuel) return;
    _modeActuel = nouveauMode;
    _panneau.effacer();
    _ecran.afficherMode(_modeActuel);
}

void GestionnaireAffiche::signalerUrgence() {
    _urgenceSignalee = true;
}

ModeAffiche GestionnaireAffiche::obtenirMode() const {
    return _modeActuel;
}

void GestionnaireAffiche::_lireCommandeSerie() {
    if (!Serial.available()) return;
    switch (Serial.read()) {
        case '1': changerMode(MODE_RABAIS); break;
        case '2': changerMode(MODE_NORMAL); break;
        case '3': changerMode(MODE_ERREUR); break;
        case '4': changerMode(MODE_FERMER); break;
    }
}

void GestionnaireAffiche::_traiterUrgence() {
    if (_modeActuel != MODE_URGENCE) {
        _modeAvantUrgence = _modeActuel;
        _modeActuel       = MODE_URGENCE;
        _panneau.mettreAJour(MODE_URGENCE);
        _ecran.afficherMode(MODE_URGENCE);
    } else {
        _panneau.effacer();
        changerMode(_modeAvantUrgence);
    }
}
