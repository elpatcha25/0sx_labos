#ifndef ETAT_AFFICHE_H
#define ETAT_AFFICHE_H

#include <Arduino.h>

enum class EtatAffiche : uint8_t {
    NORMAL,
    RABAIS,
    ERREUR,
    FERMER,
    URGENCE
};

#endif
