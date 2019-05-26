#define interupteur 7
#define moteur 10
#include <Servo.h>      //library servo

/*servo*/
Servo monServo1;
Servo monServo2;
int po = 0;
String message = "";            //message= chaine de caractère de base qui est envoyé
String texte;                   //texte= chaine de caractère recue par le serveur

/*ultrasons*/
const byte TRIGGER_PIN = 2;   // Broche TRIGGER
const byte ECHO_PIN = 3;      // Broche ECHO
const unsigned long MEASURE_TIMEOUT = 25000UL;    // 25ms = ~8m à 340m/s Constantes pour le timeout
const float SOUND_SPEED = 340.0 / 1000;           //Vitesse du son dans l'air en mm/us

long randNumber;  //direction au hasard

/*servomoteurs*/
void servo1left()   //fonction qui dirige le servo1 à gauche
{
  for (po = 1; po < 90; po++)
  {
    monServo1.write(po);
  }
}
void servo1down()   //fonction qui dirige le servo1 en arrière
{
  for (po = 90; po < 180; po++)
  {
    monServo1.write(po);
  }
}
void servo2right()    //fonction qui dirige le servo2 à droite
{
  for (po = 1; po < 90; po++)
  {
    monServo2.write(po);
  }
}
void servo2up()     //fonction qui dirige le servo2 en haut
{
  for (po = 90; po < 180; po++)
  {
    monServo2.write(po);
  }
}


/*wifi*/
void AT(String mes)
{
  Serial1.print(mes);
  Serial1.write(13);    //caractères invisibles indispensables pour l'envoie des requètes
  Serial1.write(10);
}

void envMessage()   //fonction qui envoie le message
{
  int nbCar = message.length();
  AT("AT+CIPSEND=" + String(nbCar));
  AttendConnect("OK");
  Serial1.print(message);
  //Serial1.write(13);
  //Serial1.write(10);
  Serial.println();
}


void recMessage()   //fonction qui recoit les réponses du serveur
{
  texte = "";   //réinitialisation du message envoyé au cas ou il prends une valeure avant (GPS)
  do
    if (Serial1.available() > 0)
    {
      char car = Serial1.read();
      if (car == '+')
        do
        {
          if (Serial1.available() > 0) car = Serial1.read();
        }
        while (car != ':');
      else if (car > 32)    // enleve les caractes ascii invisibles
        texte += car;
    }
  while (texte.indexOf("CLOSE") == -1);
  while (Serial1.available() > 0) Serial1.read();
  Serial.print(texte.substring(texte.indexOf("direction"), (texte.indexOf("</div>"))));
  Serial.print("   ");    //je ne garde que la partie de réponse du serveur qui me concèrne (direction)
  Serial.print(texte.substring(texte.indexOf("mode"), (texte.indexOf("Fin"))));
  Serial.print("   ");    //je ne garde que la partie de réponse du serveur qui me concèrne (mode)
}

void AttendConnect(String mot)
{
  texte = "";
  do
    if (Serial1.available() > 0)
    {
      char car = Serial1.read();
      texte += car;
      //Serial.print(car);
    }

  while (texte.indexOf(mot) == -1);
  while (Serial1.available() > 0) Serial1.read();
}


void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600);

  pinMode(interupteur, INPUT_PULLUP);   //interupteur défini
  pinMode(moteur, OUTPUT);    //moteur (direction) définie

  Serial.println("Demarrage...");
  Serial.println("Connection au wifi...");
  AT("AT+CWJAP=\"Wifiprojet\",\"projet123\"");  //connexion au Wifi
  AttendConnect("OK");
  Serial.println("Wifi connecté");

  /*ultason*/
  pinMode(TRIGGER_PIN, OUTPUT);   //Initialise les broches capt ultason
  digitalWrite(TRIGGER_PIN, LOW); // La broche TRIGGER doit être à LOW au repos
  pinMode(ECHO_PIN, INPUT);       //Initialise les broches capt ultason

  /*servomoteurs*/
  monServo1.attach(A4);
  monServo2.attach(A2);
  monServo1.write(po);
  monServo2.write(po);

  randomSeed(analogRead(0));    //direction random

  Serial.println("...........fin du void setup...........");
}



void loop()
{
  AT("AT+CIPSTART=\"TCP\",\"172.16.13.4\",80"); //connexion au serveur
  AttendConnect("OK");
  envMessage();  // Envoie de la requete au serveur
  recMessage();  // reception de la reponse du serveur
  AT("");
  delay(300);

  /*MODE MANUEL*/
  if (texte.substring(texte.indexOf("mode"), (texte.indexOf("Fin")))  == "mode6")   //si le mode est manuel
    if (digitalRead(interupteur) == 0)    //si l'interupteur l'autorise
  {
    Serial.println("Mode manuel (interupteur à 0)");
    digitalWrite(moteur, HIGH);     //faire tourner le moteur
    if (texte.substring(texte.indexOf("direction"), (texte.indexOf("</div>")))  == "direction2")
    { //si le serveur dit d'aller à gauche
      servo1left();     //orienter le moteur à gauche
      Serial.println("servo1left()");

    }
    if (texte.substring(texte.indexOf("direction"), (texte.indexOf("</div>")))  == "direction4")
    { //si le serveur dit d'aller en arrière
      servo1down();     //orienter le moteur en arrière
      Serial.println("servo1down()");

    }
    if (texte.substring(texte.indexOf("direction"), (texte.indexOf("</div>")))  == "direction1")
    { //si le serveur dit d'aller en avant
      servo2up();     //orienter le moteur en avant
      Serial.println("servo2up()");
    }
    if (texte.substring(texte.indexOf("direction"), (texte.indexOf("</div>")))  == "direction3")
    { //si le serveur dit d'aller à droite
      servo2right();     //orienter le moteur à droite
      Serial.println("servo2right()");

    }
    if (texte.substring(texte.indexOf("direction"), (texte.indexOf("</div>")))  == "direction0")
    {{ //si le serveur dit d'arreter le moteur
        digitalWrite(moteur, LOW);     //arreter le moteur
      }
    }


    digitalWrite(TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGGER_PIN, LOW);
    long measure = pulseIn(ECHO_PIN, HIGH, MEASURE_TIMEOUT);       //Mesure le temps entre l'envoi de l'impulsion ultrasonique et son écho (si il existe)
    float distance_mm = measure / 2.0 * SOUND_SPEED;    //Calcul la distance à partir du temps mesuré

    /*MODE AUTAMATIQUE*/
    if (texte.substring(texte.indexOf("mode"), (texte.indexOf("Fin")))  == "mode5")   //si le mode est automatique
      if (digitalRead(interupteur) == 1)    //si l'interupteur l'autorise
    {
      Serial.println("Mode automatique (interupteur à 1)");
      digitalWrite(moteur, HIGH);
      if (distance_mm > 1)    //empèche les faux contactes de tourner inutilement le moteur
      {
        Serial.println(distance_mm);      //distance en mm s'écris dans le moniteur
      }
      delay(100); //Délai d'attente pour éviter d'afficher trop de résultats à la seconde

      if (distance_mm < 200 && distance_mm > 1)   //si le capteur de distance repère un obstacle à moins de 20cm
      {
        randNumber = random(1, 4);    //generer un numéro aléatoire entre 1et 4 compris
        if (randNumber == 1)      // si le numéro aléatoire = 1
        {
          servo2up();     //orienter le moteur en avant
          Serial.println("servo2up()");
        }
        if (randNumber == 2)      // si le numéro aléatoire = 2
        {
          servo1left();     //orienter le moteur à gauche
          Serial.println("servo1left()");

        }
        if (randNumber == 3)      // si le numéro aléatoire = 3
        {
          servo2right();     //orienter le moteur à droite
          Serial.println("servo2right()");

        }
        if (randNumber == 4)      // si le numéro aléatoire = 4
        {
          servo1down();     //orienter le moteur en bas
          Serial.println("servo1down()");

        }

      }

    }
  }
