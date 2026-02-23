// TEMATIO TSAKENG
// DA: 2407822


const int BROCHE_POTENTIOMETRE = A1;
const int BUTTON_PIN = 2;
const int NOMBRE_LEDS = 4;
const int LONGUEUR_BARRE = 20;
const int BROCHE_LEDS[NOMBRE_LEDS] = { 7, 8, 9, 10 };
const int VALEUR_MIN = 0;
const int VALEUR_MAX = 1023;
const int POURCENTAGE_MAX = 100;
const int VALEUR_MAPPEE_MAX = 20;

unsigned long currentTime = 0;
int potentialValue = 0;
int mapValue = 0;


int estClic(unsigned long ct) {
  static unsigned long lastTime = 0;
  static int lastState = HIGH;
  const int rate = 50;
  int clic = 0;

  if (ct - lastTime < rate) {
    return clic;  // Trop rapide
  }

  lastTime = ct;

  int state = digitalRead(BUTTON_PIN);

  if (state == LOW && lastState == HIGH) {
    clic = 1;
  }

  lastState = state;
  return clic;
}


void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  for (int i = 0; i < NOMBRE_LEDS; i++) {
    pinMode(BROCHE_LEDS[i], OUTPUT);
  }

  Serial.println("Setup completed");
}


void loop() {
  currentTime = millis();
  allumerLed();
  afficherBarre();
}


void allumerLed() {
  potentialValue = analogRead(BROCHE_POTENTIOMETRE);
  mapValue = map(potentialValue, VALEUR_MIN, VALEUR_MAX, VALEUR_MIN, VALEUR_MAPPEE_MAX);
  
  int pourcentage = map(potentialValue, VALEUR_MIN, VALEUR_MAX, VALEUR_MIN, POURCENTAGE_MAX);

  int ledActive;

  if (pourcentage <= 25) {
    ledActive = 0;
  } else if (pourcentage > 25 && pourcentage <=50) {
    ledActive = 1;
  } else if (pourcentage > 50 && pourcentage <=75) {
    ledActive = 2;
  } else {
    ledActive = 3;
  }

  for (int i = 0; i < NOMBRE_LEDS; i++) {
    if (i == ledActive) {
      digitalWrite(BROCHE_LEDS[i], HIGH);
    } else {
      digitalWrite(BROCHE_LEDS[i], LOW);
    }
  }
}

void afficherBarre() {
  int pourcentage = map(potentialValue, VALEUR_MIN, VALEUR_MAX, VALEUR_MIN, POURCENTAGE_MAX);

  if (estClic(currentTime)) {
    Serial.print("[");
    for (int i = 0; i < LONGUEUR_BARRE; i++) {
      if (i < mapValue) {
        Serial.print("!");
      } else {
        Serial.print(".");
      }
    }
    Serial.print("] ");
    Serial.print(pourcentage);
    Serial.println("%");
  }
}