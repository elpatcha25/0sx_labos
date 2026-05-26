#pragma once

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "Etats.h"

class EcranLCD {
public:
    EcranLCD(uint8_t adresse, uint8_t colonnes, uint8_t rangees);
    void demarrer();
    void afficherMode(ModeAffiche etat);

private:
    LiquidCrystal_I2C _ecran;
    ModeAffiche       _dernierEtat;
};

