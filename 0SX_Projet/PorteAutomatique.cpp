#include "PorteAutomatique.h"

PorteAutomatique::PorteAutomatique(int broche)
    : _broche(broche),
      _etat(PORTE_FERMEE),
      _angle(ANGLE_FERME),
      _tempsOuverture(0),
      _dernierServo(0)
{}

void PorteAutomatique::demarrer() {
    _servo.attach(_broche);
    _servo.write(ANGLE_FERME);
    _angle = ANGLE_FERME;
}

void PorteAutomatique::mettreAJour(unsigned long maintenant, float distanceCm) {
    if (_etat == PORTE_URGENCE) return;
    _gererEtat(maintenant, distanceCm);
    _deplacerServo(maintenant);
}

void PorteAutomatique::ouvrirManuellement() {
    if (_etat == PORTE_FERMEE) {
        _etat = PORTE_OUVERTURE;
    } else if (_etat == PORTE_FERMETURE) {
        _etat = PORTE_OUVERTURE;
        _angle++;
    }
}

void PorteAutomatique::activerUrgence() {
    _etat = PORTE_URGENCE;
}

void PorteAutomatique::desactiverUrgence() {
    _etat  = PORTE_FERMEE;
    _angle = ANGLE_FERME;
    _servo.write(ANGLE_FERME);
}

int PorteAutomatique::lireAngle() const {
    return _angle;
}

EtatPorte PorteAutomatique::lireEtat() const {
    return _etat;
}

const char* PorteAutomatique::lireEtatTexte() const {
    switch (_etat) {
        case PORTE_FERMEE:    return "Fermee";
        case PORTE_OUVERTURE: return "Ouverture...";
        case PORTE_OUVERTE:   return "Ouverte";
        case PORTE_FERMETURE: return "Fermeture...";
        case PORTE_URGENCE:   return "URGENCE";
        default:              return "???";
    }
}

void PorteAutomatique::_gererEtat(unsigned long maintenant, float distanceCm) {
    switch (_etat) {
        case PORTE_FERMEE:
            if (distanceCm > 0 && distanceCm < SEUIL_DISTANCE_PORTE) {
                _etat = PORTE_OUVERTURE;
            }
            break;

        case PORTE_OUVERTURE:
            if (_angle >= ANGLE_OUVERT) {
                _etat = PORTE_OUVERTE;
                _tempsOuverture = maintenant;
            }
            break;

        case PORTE_OUVERTE:
            if (maintenant - _tempsOuverture >= DELAI_OUVERTURE) {
                _etat = PORTE_FERMETURE;
            }
            break;

        case PORTE_FERMETURE:
            if (_angle <= ANGLE_FERME) {
                _etat = PORTE_FERMEE;
            }
            break;

        case PORTE_URGENCE:
            break;
    }
}

void PorteAutomatique::_deplacerServo(unsigned long maintenant) {
    if (maintenant - _dernierServo < DELAI_SERVO) return;
    _dernierServo = maintenant;

    if (_etat == PORTE_OUVERTURE && _angle < ANGLE_OUVERT) {
        _angle++;
        _servo.write(_angle);
    } else if (_etat == PORTE_FERMETURE && _angle > ANGLE_FERME) {
        _angle--;
        _servo.write(_angle);
    }
}
