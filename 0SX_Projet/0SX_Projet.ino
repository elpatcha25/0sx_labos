#include <IRremote.hpp>
#include <Wire.h>
#include <HCSR04.h>
#include "Etats.h"
#include "PorteAutomatique.h"
#include "Convoyeur.h"
#include "GestionnaireAffiche.h"
#include "ClientMQTT.h"
#include <LCD_I2C.h>

//const char* WIFI_SSID      = "TechniquesInformatique-Etudiant";
//const char* WIFI_MOT_PASSE = "shawi123";
const char* WIFI_SSID      = "BELL660";
const char* WIFI_MOT_PASSE = "A2F72E699917";
const char* NUM_ETUDIANT   = "13";

const int PIN_SERVO    = 9;
const int PIN_TRIGGER  = 12;
const int PIN_ECHO     = 11;
const int PIN_BTN_OPEN = 4;

const int PIN_JOY_Y       = A0;
const int PIN_JOY_X       = A1;
const int PIN_MOTEUR_IN1  = 44;
const int PIN_MOTEUR_IN2  = 45;
const int PIN_MOTEUR_EN   = 5;
const int PIN_LED_ACTIVE  = 6;
const int PIN_LED_URG     = 7;

const int PIN_IR  = 8;
const int PIN_DIN = 34;
const int PIN_CLK = 30;
const int PIN_CS  = 32;

const int PIN_BTN_URG = 3;

const int CENTRE_JOY     = 512;
const int ZONE_MORTE_JOY = 150;
const unsigned long DELAI_VITESSE  = 80;
const unsigned long DELAI_PAGE_LCD = 2000;
const unsigned long DEBOUNCE       = 200;

HCSR04              capteur(PIN_TRIGGER, PIN_ECHO);
PorteAutomatique    porte(PIN_SERVO);
Convoyeur           convoyeur(PIN_MOTEUR_IN1, PIN_MOTEUR_IN2,
                              PIN_MOTEUR_EN, PIN_LED_ACTIVE, PIN_LED_URG);
GestionnaireAffiche affiche(PIN_DIN, PIN_CLK, PIN_CS);
ClientMQTT          mqtt(WIFI_SSID, WIFI_MOT_PASSE, NUM_ETUDIANT);
LCD_I2C             lcd(0x27, 16, 2);

bool          modeUrgence      = false;
EtatConvoyeur etatConvoyeur    = CONV_ARRET;
ModeAffiche   modeAffiche      = MODE_NORMAL;
float         distanceCm       = 0.0;
bool          btnUrgPrecedent  = HIGH;
bool          btnOpenPrecedent = HIGH;
unsigned long dernierAjustVit  = 0;
unsigned long dernierBtnUrg    = 0;
unsigned long dernierChangeLCD = 0;
int           pageLCD          = 0;

void setup() {
    Serial.begin(9600);
    pinMode(PIN_BTN_URG,  INPUT_PULLUP);
    pinMode(PIN_BTN_OPEN, INPUT_PULLUP);

    IrReceiver.begin(PIN_IR, DISABLE_LED_FEEDBACK);

    porte.demarrer();
    convoyeur.debut();
    affiche.demarrer();
    mqtt.demarrer();

    lcd.begin();
    lcd.backlight();
    lcd.clear();
    lcd.print("Systeme pret");
    delay(1500);
    lcd.clear();
}

void loop() {
    unsigned long maintenant = millis();

    static unsigned long derniereDist = 0;
    if (maintenant - derniereDist >= 50) {
        distanceCm = capteur.dist();
        derniereDist = maintenant;
    }

    bool btnUrg = digitalRead(PIN_BTN_URG);
    if (btnUrgPrecedent == HIGH && btnUrg == LOW &&
        (maintenant - dernierBtnUrg) > DEBOUNCE) {
        dernierBtnUrg = maintenant;
        gererUrgence();
    }
    btnUrgPrecedent = btnUrg;

    bool btnOpen = digitalRead(PIN_BTN_OPEN);
    if (btnOpenPrecedent == HIGH && btnOpen == LOW && !modeUrgence) {
        porte.ouvrirManuellement();
    }
    btnOpenPrecedent = btnOpen;

    lireIR();
    gererCommandesMQTT();

    if (!modeUrgence) {
        porte.mettreAJour(maintenant, distanceCm);
        gererConvoyeur(maintenant);
        affiche.mettreAJour();
    }

    afficherLCD(maintenant);
    mqtt.mettreAJour(porte.lireEtat(), distanceCm,
                     etatConvoyeur, convoyeur.lireVitesse(), modeAffiche);
}

void lireIR() {
    if (!IrReceiver.decode()) return;
    if (IrReceiver.decodedIRData.decodedRawData != 0 && !modeUrgence) {
        uint8_t code = IrReceiver.decodedIRData.command;
        switch (code) {
            case BTN_IR_RABAIS: changerModeAffiche(MODE_RABAIS); break;
            case BTN_IR_NORMAL: changerModeAffiche(MODE_NORMAL); break;
            case BTN_IR_ERREUR: changerModeAffiche(MODE_ERREUR); break;
            case BTN_IR_FERMER: changerModeAffiche(MODE_FERMER); break;
        }
    }
    IrReceiver.resume();
}

void changerModeAffiche(ModeAffiche m) {
    modeAffiche = m;
    affiche.changerMode(m);
}

void gererUrgence() {
    if (!modeUrgence) {
        modeUrgence   = true;
        modeAffiche   = MODE_URGENCE;
        etatConvoyeur = CONV_URGENCE;
        convoyeur.stopper();
        convoyeur.allumerLedUrgence(true);
        porte.activerUrgence();
        affiche.signalerUrgence();
        lcd.clear();
        lcd.setCursor(3, 0);
        lcd.print("!! URGENCE !!");
        lcd.setCursor(2, 1);
        lcd.print("Appui = reset");
    } else {
        modeUrgence   = false;
        modeAffiche   = MODE_NORMAL;
        etatConvoyeur = CONV_ARRET;
        convoyeur.allumerLedUrgence(false);
        porte.desactiverUrgence();
        affiche.signalerUrgence();
        lcd.clear();
    }
}

void gererCommandesMQTT() {
    if (mqtt.urgencePending() && !modeUrgence) {
        gererUrgence();
        mqtt.resetPending();
        return;
    }
    if (!modeUrgence) {
        ModeAffiche m = mqtt.lireModePending();
        if (m != modeAffiche) {
            changerModeAffiche(m);
            mqtt.resetPending();
        }
    }
}

void gererConvoyeur(unsigned long maintenant) {
    switch (etatConvoyeur) {
        case CONV_ARRET: {
            int deltaY = analogRead(PIN_JOY_Y) - CENTRE_JOY;
            if (abs(deltaY) > ZONE_MORTE_JOY) {
                etatConvoyeur = CONV_MARCHE;
                appliquerDirection(deltaY);
            }
            break;
        }
        case CONV_MARCHE: {
            int deltaY = analogRead(PIN_JOY_Y) - CENTRE_JOY;
            int deltaX = analogRead(PIN_JOY_X) - CENTRE_JOY;
            if (abs(deltaX) > ZONE_MORTE_JOY &&
                (maintenant - dernierAjustVit) >= DELAI_VITESSE) {
                dernierAjustVit = maintenant;
                convoyeur.modifierVitesse(deltaX);
            }
            if (abs(deltaY) <= ZONE_MORTE_JOY) {
                convoyeur.stopper();
                etatConvoyeur = CONV_ARRET;
            } else {
                appliquerDirection(deltaY);
            }
            break;
        }
        case CONV_URGENCE: break;
    }
}

void appliquerDirection(int deltaY) {
    if (deltaY > 0) convoyeur.allerAvant();
    else            convoyeur.allerArriere();
}

void afficherLCD(unsigned long maintenant) {
    if (modeUrgence) return;
    if (maintenant - dernierChangeLCD >= DELAI_PAGE_LCD) {
        dernierChangeLCD = maintenant;
        pageLCD = (pageLCD + 1) % 3;
        lcd.clear();
    }
    switch (pageLCD) {
        case 0:
            lcd.setCursor(0, 0);
            lcd.print("Dist:");
            lcd.print((int)distanceCm);
            lcd.print("cm          ");
            lcd.setCursor(0, 1);
            lcd.print(porte.lireEtatTexte());
            lcd.print("            ");
            break;
        case 1:
            lcd.setCursor(0, 0);
            lcd.print("Conv: ");
            lcd.print(convoyeur.estEnMarche()
                ? (convoyeur.estEnAvant() ? "AVANT   " : "ARRIERE ")
                : "ARRET   ");
            lcd.setCursor(0, 1);
            lcd.print("Vit: ");
            lcd.print(convoyeur.lireVitesse());
            lcd.print("/255       ");
            break;
        case 2:
            lcd.setCursor(0, 0);
            lcd.print("Mode:           ");
            lcd.setCursor(0, 1);
            switch (modeAffiche) {
                case MODE_NORMAL:  lcd.print("NORMAL          "); break;
                case MODE_RABAIS:  lcd.print("RABAIS          "); break;
                case MODE_ERREUR:  lcd.print("ERREUR          "); break;
                case MODE_FERMER:  lcd.print("FERME           "); break;
                case MODE_URGENCE: lcd.print("URGENCE         "); break;
            }
            break;
    }
}
