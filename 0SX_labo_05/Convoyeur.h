#ifndef CONVOYEUR_H
#define CONVOYEUR_H

#include <Arduino.h>

class Convoyeur {
public:
  Convoyeur(int brancheEntree1, int brancheEntree2,
            int brancheActivation,
            int brancheLedActive, int brancheLedUrgence);

  void debut();

  void allerAvant();
  void allerArriere();
  void stopper();
  void modifierVitesse(int valeurX);

  bool estEnMarche()  const;
  bool estEnAvant()   const;
  int  lireVitesse()  const;

  void allumerLedUrgence(bool allumee);

private:
  int _brancheEntree1;
  int _brancheEntree2;
  int _brancheActivation;
  int _brancheLedActive;
  int _brancheLedUrgence;

  bool _enMarche;
  bool _enAvant;
  int  _vitesse;

  void _mettreAjourLed();
  void _envoyerVitesse();
};

#endif
