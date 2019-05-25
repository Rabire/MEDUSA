#include <SPI.h>
#include <Ethernet2.h>
#include <Server.h>
#include <Client.h>
#define Led 13
int direc ;
int mode ;


//--- Paramètres du serveur ---
byte mac[] = { 0x90, 0xA2, 0xDA, 0x10, 0xF7, 0x63 };  //addresse MAC du shield Arduino Ethernet
byte ip[] = { 172, 16, 13, 4 };   //172.16.10.4       //addresse du serveur
byte passerelle[] = { 172, 16, 0, 252};
byte masque[] = { 255, 255, 0, 0 };
EthernetServer serveurMinip(80);

String chaineRecue = ""; // déclare un string vide global
int comptChar = 0; // variable de comptage des caractères reçus


void setup()
{
  Serial.begin(9600);
  Ethernet.begin(mac, ip, passerelle, masque);
  serveurMinip.begin();
}

void loop()
{
  while (!serveurMinip.available());
  EthernetClient client = serveurMinip.available();

  if (client)
  {
    chaineRecue = "";
    comptChar = 0;

    if (client.connected())
    {
      //////////////// Reception de la requete envoyée par le client //////////////////

      while (client.available())
      {
        char c = client.read();
        comptChar++;
        if (comptChar > 130) chaineRecue = chaineRecue + c;
      }


      if (chaineRecue.indexOf("appU") != -1) {      //ici, quand je recois appU, "direc" prends la valeure 1
        Serial.println("requête appU recue");
        direc = 1; //Up
      }
      if (chaineRecue.indexOf("appL") != -1) {    //Quand je recois appL, "direc" prends la valeure 2
        Serial.println("requête appL recue");
        direc = 2; //Left
      }
      if (chaineRecue.indexOf("appR") != -1) {    //Quand je recois appR, "direc" prends la valeure 3
        Serial.println("requête appR recue");
        direc = 3; //Right
      }
      if (chaineRecue.indexOf("appD") != -1) {    //Quand je recois appD, "direc" prends la valeure 4
        Serial.println("requête appD recue");
        direc = 4; //Down
      }
      if (chaineRecue.indexOf("appS") != -1) {    //Quand je recois appS, "direc" prends la valeure 0
        Serial.println("requête appS recue");
        direc = 0; //Stop
      }

      if (chaineRecue.indexOf("ModeManu") != -1) {    //ici, quand je recois appU, "mode" prends la valeure 6
        Serial.println("requête ModeManu recue");
        mode = 5;
      }
      if (chaineRecue.indexOf("ModeAuto") != -1) {    //Quand je recois appS, "mode" prends la valeure 5
        Serial.println("requête ModeAuto recue");
        mode = 6;
      }
      if (chaineRecue.indexOf("ModeInterup") != -1) { //Quand je recois appS, "mode" prends la valeure 7
        Serial.println("requête ModeInterup recue");
        mode = 7;
      }




      // envoi d'un entete standard de réponse http
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println("Access-Control-Allow-Origin: *");
      client.println("Connection: close");
      client.println();

      //entete de la page html avec ragrechissement automatique toutes les 0.5s
      client.print("<head><meta http-equiv=\"refresh\" content=\"0.5;url=http://172.16.13.4\" /></head>");
      //la div permet au collègue qui s'occupe du site de lire le contenu plus facilement
      client.print("<div id=\"kardech\">");
      
      client.print("direction");
      client.print(direc);        //= direction0; direction1; direction2; direction3; direction4; 
      
      client.print("</div>");   //fin div id=kardech
      
      client.print("mode");
      client.print(mode);

      client.print("Fin");    //ce mot me permet d'encadrer la valeure du mode et de la lire plus facilement

      delay(1);
      client.stop();
    }
  }
}
