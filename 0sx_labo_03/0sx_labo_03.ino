//TEMATIO TSAKEMG
#include <Wire.h>
#include <LCD_I2C.h>

LCD_I2C lcd(0x27, 16, 2); // Adresse I2C 0x27, écran 16x2

const int LED_CLIM = 8;
const int BOUTON_MODE = 2;
float RESISTANCE_REF = 10000; // Résistance de référence
float logResistance, resistanceNTC, temperatureKelvin, temperatureCelsius;

float coeff1 = 1.129148e-03, coeff2 = 2.34125e-04, coeff3 = 8.76741e-08; // Coefficients Steinhart-Hart
int etatClim = 0; // 0 = OFF, 1 = ON
const String numEtudiant = "2407822";
unsigned long dernierMajTemperature = 0;
unsigned long dernierMajSerie = 0;
unsigned long dernierAppuiBouton = 0;
unsigned long dernierMajLCD = 0;
int altitude = 0;
int nMax = 200;
int valSup = 600;
int valInf = 400;
int modeAffichage = 0; // 0 = Température, 1 = Contrôle Joystick
bool etatBouton = HIGH, etatBoutonPrecedent = HIGH;

byte caractereNumero[8] = {
  B01000,
  B10100,
  B00100,
  B01010,
  B11101,
  B00001,
  B00010,
  B00111
};

byte caractereDegre[8] = {
  0B01000,
  0B10100,
  0B01000,
  0B00000,
  0B00000,
  0B00000,
  0B00000,
  0B00000
};

void afficherMessageDemarrage() {
    static unsigned long tempsAttente = 3000;

    lcd.createChar(0, caractereNumero);
    lcd.createChar(1, caractereDegre);

    while (millis() < tempsAttente) {
        lcd.print("TEMATIO TSAKENG");

        lcd.setCursor(0, 1);
        lcd.print(char(0));

        lcd.setCursor(9, 1);
        lcd.print("*****22");
    }
}

void setup() {
    pinMode(LED_CLIM, OUTPUT);
    pinMode(BOUTON_MODE, INPUT_PULLUP); // Activer la résistance de pull-up interne
    Serial.begin(115200);

    lcd.begin(16, 2);
    lcd.backlight();
    
    // Affichage du message de démarrage
    afficherMessageDemarrage();
    lcd.clear();
}

void mesurerTemperature() {
    static unsigned long intervalleMaj = 500;

    if (millis() - dernierMajTemperature >= intervalleMaj) { 
        dernierMajTemperature = millis();

        int valeurNTC = analogRead(A0);
        resistanceNTC = RESISTANCE_REF * (1023.0 / valeurNTC - 1.0);
        logResistance = log(resistanceNTC);
        temperatureKelvin = (1.0 / (coeff1 + coeff2 * logResistance + coeff3 * logResistance * logResistance * logResistance));
        temperatureCelsius = temperatureKelvin - 273.15;

        lcd.setCursor(0, 0);
        lcd.print("Temp: ");
        lcd.print(temperatureCelsius);
        lcd.print(" ");
        lcd.print(char(1)); 
        lcd.print("C");
      

        lcd.setCursor(0, 1);
        if (temperatureCelsius > 35) {
            digitalWrite(LED_CLIM, HIGH); 
            lcd.print("COOLING: ON  ");
            etatClim = 1;
        } else if (temperatureCelsius < 30) {
            digitalWrite(LED_CLIM, LOW);
            lcd.print("COOLING: OFF ");
            etatClim = 0;
        }
    }
}



void controlerJoystick() {
    static unsigned long intervalleMaj = 1000; // 1 seconde

    int valeurX = analogRead(A1);
    int valeurY = analogRead(A2);

    // Mise à jour altitude chaque seconde
    if (millis() - dernierMajLCD >= intervalleMaj) {
        dernierMajLCD = millis();

        if (valeurY > valSup) {
            //mon Joystick monte vers le haut
            if (altitude < nMax) {
                altitude += 1; // plus 1metre  par seconde
            }
        } else if (valeurY < valInf) {
            // Joystick descend vers le bas
            if (altitude > 0) {
                altitude -= 1; // -1metre  par seconde
            }
        }
       
    }

    // Calcul direction
    int direction = map(valeurX, 0, 1023, -90, 90);

    // Affichage ligne 1 - Altitude
    lcd.setCursor(0, 0);
    lcd.print("ALT: ");
    lcd.print(altitude);
    lcd.print("m   ");

    if (valeurY > valSup) {
        lcd.print("UP    ");
    }   
    else if (valeurY < valInf) {
        lcd.print("DOWN  ");
    }
    else {
        lcd.print("STABLE"); 
    }
    

    // Affichage ligne 2 - Direction
    lcd.setCursor(0, 1);
    lcd.print("DIR: ");
    lcd.print(direction);

    if (direction < 0) {
        lcd.print("(G)  ");
    } else {
        lcd.print("(D)  ");
    }
}


void gererBoutonMode() {
    etatBouton = digitalRead(BOUTON_MODE);

    if (etatBouton == LOW && etatBoutonPrecedent == HIGH) {
        if (millis() - dernierAppuiBouton >= nMax) { 
            dernierAppuiBouton = millis();
            modeAffichage = (modeAffichage + 1) % 2;
            lcd.clear();
        }
    }
    etatBoutonPrecedent = etatBouton;

    if (modeAffichage == 0) {
        mesurerTemperature();
    } else {
        controlerJoystick();
    }
}

void affichageSerie(){
static unsigned long intervalleEnvoiSerie = 100;

    if (millis() - dernierMajSerie >= intervalleEnvoiSerie) {

        dernierMajSerie = millis();

        int valeurX = analogRead(A1);
        int valeurY = analogRead(A2);

        Serial.print("etd:");
        Serial.print(numEtudiant);
        Serial.print(",x:");
        Serial.print(valeurX);
        Serial.print(",y:"); 
        Serial.print(valeurY);
        Serial.print(",sys:"); 
        Serial.println(etatClim);
    }
}

void loop() {
    
gererBoutonMode();
affichageSerie();
    
}
