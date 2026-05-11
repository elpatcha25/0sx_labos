#ifndef ECRAN_LCD_H
#define ECRAN_LCD_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "EtatAffiche.h"


class EcranLCD {
public:
    EcranLCD(uint8_t adresse, uint8_t colonnes, uint8_t rangees);
    void demarrer();
    void afficherMode(EtatAffiche etat);

private:
    LiquidCrystal_I2C _ecran;
    EtatAffiche       _dernierEtat;
};

#endif
