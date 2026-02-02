//enum EtatAppli {clignotement, allume_eteint, variation};
// NOM: TEMATIO TSAKENG
int led = 13;





void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(led, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:


  clignotement();
  variation();
  eteint_allume();
}
void eteint_allume(){
  int firstDelay = 300;
  int secondDelay = 2000;
  int thirdDelay = 1000;

  Serial.println("Etat:Allume - 2407822");
  digitalWrite(led, LOW);
  delay(firstDelay);
  digitalWrite(led,HIGH);
  delay(secondDelay); 
  digitalWrite(led, LOW);
  delay(thirdDelay);

}
void variation(){
  int delayTime = 2048;
  int totalVariation = 256;
  int delayPerPeriode = delayTime/totalVariation;


  Serial.println("Etat:Varie - 2407822");
    for (int i = 0; i <= 255; i++) {          
      analogWrite(led, i);
      delay(delayPerPeriode);                 
    }
  
}

void clignotement (){
  int prevNumber = 2;
  int formula = (prevNumber / 2);
  int delayBlink = 350;

  Serial.println("Etat:clignotement - 2407822");
  
  for(int i=0; i<formula; i++){
    digitalWrite(led,HIGH);
    delay(delayBlink); 
    digitalWrite(led, LOW);
    delay(delayBlink);
 }

}

