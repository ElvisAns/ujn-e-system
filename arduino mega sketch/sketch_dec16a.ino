#include <UTFT.h>
#include <URTouch.h>
#include <SPI.h>
#include <SD.h>
//#include "ujn3.c"
//==== Creating Objects
URTouch myTouch( 6, 5, 4, 3, 2);
UTFT myLcd(ILI9481, 38, 39, 40, 41); //Parameters should be adjusted to your Display/Schield model
//URTo
extern uint8_t SmallFont[];
extern uint8_t BigFont[];
extern uint8_t SevenSegNumFont[];
extern uint8_t UbuntuBold[];
extern uint8_t CalibriBold[];

extern unsigned short ujn4[];
extern unsigned short ujn4_1[];
extern unsigned short wifi[];
//extern unsigned short error[];
//extern unsigned int fill[];
//extern unsigned int insufisant[];
//extern unsigned int serverdown[];
//extern unsigned short late[];

int x, y;
char currentPage, selectedUnit;
String total_presences = "";
#define SD_CS 53  //SD card pin on your shield

bool hasDisplayed = false;
int continuec = 0;

void setup() {
  // Initial setup
  Serial1.begin(9600); //serie - nodemcu
  Serial.begin(9600); //moniteur
  myLcd.InitLCD(0);
  //myLcd.setXY(0,480);
  myLcd.clrScr();
  //myTouch.InitTouch();
  //myTouch.setPrecision(PREC_MEDIUM);

  myLcd.fillScr(VGA_WHITE);
  myLcd.setBackColor(VGA_WHITE);
  myLcd.drawBitmap(100, 300, 100, 100, wifi);
  myLcd.setColor(255, 0, 0);
  myLcd.setFont(BigFont);

  delay(500);
  while (continuec == 0) {
    String mydata;
    while (Serial1.available()) {
      mydata = Serial1.readStringUntil('\n');
    }
    Serial.println("wait...");
    if (mydata.indexOf("off") > -1) {
      Serial.println("Connexion bado");
      continuec = 0;
      if (!hasDisplayed) {
        hasDisplayed = true;
        myLcd.fillScr(VGA_WHITE);
        myLcd.setBackColor(VGA_WHITE);
        myLcd.drawBitmap(100, 300, 100, 100, wifi);
        myLcd.setColor(255, 0, 0);
        myLcd.setFont(BigFont);
        myLcd.print("En attente Reseau", CENTER, 50);
      }
    }
    else if (mydata.indexOf("connec") > -1) {
      continuec = 1;
      myLcd.drawBitmap(100, 300, 100, 100, wifi);
      myLcd.fillScr(VGA_WHITE);
      myLcd.print("        Connected....         ", CENTER, 50); // Prints the string on the screen
    }
    mydata = "";
    delay(50);
  }
  delay(2000);
  if (!SD.begin(SD_CS)) {
    myLcd.clrScr();
    myLcd.fillScr(VGA_WHITE);
    myLcd.setBackColor(VGA_WHITE);
    //myLcd.drawBitmap(100, 80, 100, 100, sdcarderror);
    myLcd.setColor(255, 0, 0); // Sets color to white
    myLcd.setFont(UbuntuBold); // Sets font to big
    myLcd.print("No SD-Card", CENTER, 300); // Prints the string on the screen
    myLcd.drawLine(20, 55, 310, 55);
    delay(2000);
    myLcd.setFont(BigFont);
    myLcd.setColor(0, 0, 255);
    myLcd.print("Initialisation..", CENTER, 100);
    myLcd.setFont(UbuntuBold);
    delay(1000);
    myLcd.setColor(0, 255, 0);
    myLcd.print(".", CENTER, 150);
    delay(500);
    myLcd.setFont(BigFont);
    myLcd.print("                             ", CENTER, 100);
    myLcd.setColor(0, 0, 0);
    myLcd.print("Vous utilisez", CENTER, 100);
    myLcd.print("la Memoire interne", CENTER, 70);
    myLcd.setFont(UbuntuBold);
    myLcd.setColor(0, 255, 0);
    myLcd.print("..", CENTER, 150);
    delay(500);
    myLcd.print("...", CENTER, 150);
    delay(2000);
    //myLcd.drawBitmap(100, 80, 100, 100, fill);
  }
  myLcd.fillScr(VGA_WHITE);
  drawStartScreen();
  currentPage = '0'; // Indicates that we are at Home Screen
  selectedUnit = '0'; // Indicates the selected unit for the first example, cms or inches
  myLcd.clrScr();
  myLcd.fillScr(VGA_WHITE);
  myLcd.setBackColor(VGA_WHITE);
}
void (*reset_func)(void) = 0;
void loop() {
  String mydata = "";
  while (Serial1.available()) {
    mydata = Serial1.readStringUntil('\n');
  }

  if (mydata.indexOf("off") > -1) {
    delay(500);
    reset_func();
  }




  myLcd.drawBitmap(2, 440, 40, 37, ujn4_1);
  myLcd.setColor(0, 0, 255); // Sets color to white
  myLcd.setFont(UbuntuBold); // Sets font to big
  myLcd.print("CLASSE:1eA", CENTER, 440); // Prints the string on the screen
  myLcd.setColor(0, 0, 0);
  myLcd.drawLine(10, 435, 310, 435);

  myLcd.setColor(0, 0, 0); // Sets color to white
  myLcd.setFont(BigFont); // Sets font to big
  myLcd.print("Placez votre carte", CENTER, 50); // Prints the string on the screen

  myLcd.setColor(0, 0, 0);
  myLcd.drawLine(40, 40, 290, 40);
  myLcd.setColor(0, 0, 255); // Sets color to white
  myLcd.setFont(BigFont); // Sets font to big
  String presences = "Presences : " + String(total_presences);
  myLcd.print(presences, CENTER, 20); // Prints the string on the screen


  myLcd.drawLine(20, 120, 300, 120);
  myLcd.drawLine(20, 425, 300, 425);

  myLcd.drawLine(20, 120, 20, 425);
  myLcd.drawLine(300, 120, 300, 425);

  String data = mydata;
  Serial.print(data);
  //data = Serial1.readString();
  // if(data.length()>1){

  if (data.length() >= 30) {
    String title = getValue(data, '&', 0);
    String nom = getValue(data, '&', 1);
    String postnom = getValue(data, '&', 2);
    String prenom = getValue(data, '&', 3);
    String classe = getValue(data, '&', 4);
    total_presences = getValue(data, '&', 5);
    myLcd.drawBitmap(100, 300, 100, 100, ujn4);
    show(" Successfull ");
    show_details(nom, postnom, prenom, classe);
    myLcd.drawRect(100, 400, 100, 300);
    myLcd.fillRect(100, 400, 100, 300);
  }
  else {
    if (data == "deactivateddeactivated") {
      myLcd.drawBitmap(100, 300, 100, 100, ujn4);
      show("ID desactivee");
      myLcd.drawRect(100, 400, 100, 300);
      myLcd.fillRect(100, 400, 100, 300);
    }
    else if (data == "already-read") {
      myLcd.drawBitmap(100, 300, 100, 100, ujn4);
      show("Deja present");
      myLcd.drawRect(100, 400, 100, 300);
      myLcd.fillRect(100, 400, 100, 300);
    }
    else if (data == "invalid-att") {
      myLcd.drawBitmap(100, 300, 100, 100, ujn4);
      show("Solde minum");
      myLcd.drawRect(100, 400, 100, 300);
      myLcd.fillRect(100, 400, 100, 300);
    }
    else if (data == "not-found" || data == "invalid") {
      myLcd.drawBitmap(100, 300, 100, 100, ujn4);
      show("Invalid card ");
      myLcd.drawRect(100, 400, 100, 300);
      myLcd.fillRect(100, 400, 100, 300);
    }
    else if (data == "invalid-time") {
      myLcd.drawBitmap(100, 300, 100, 100, ujn4);
      show(" En retard... ");
      myLcd.drawRect(100, 400, 100, 300);
      myLcd.fillRect(100, 400, 100, 300);
    }
    else if (data == "system-down") {
      myLcd.drawBitmap(100, 300, 100, 100, ujn4);
      show(" Server-down ");
      myLcd.drawRect(100, 400, 100, 300);
      myLcd.fillRect(100, 400, 100, 300);
    }
    else if (data == "network-off") {
      //myLcd.clrScr();
      myLcd.fillScr(VGA_WHITE);
      myLcd.setBackColor(VGA_WHITE);
      myLcd.drawBitmap(100, 300, 100, 100, wifi);
      myLcd.setColor(255, 0, 0);
      myLcd.setFont(BigFont);
      myLcd.print("En attente Reseau", CENTER, 50);
    }
  }
  delay(1000);
}
void drawStartScreen() {
  //draw image1 SUPRAELCTRONICS;
  //draw image2, UJN;
  //myLcd.drawBitmap();
  // Title

  myLcd.fillScr(VGA_WHITE);
  myLcd.setBackColor(VGA_WHITE);
  myLcd.setColor(0, 0, 0); // Sets color to red
  myLcd.setFont(SmallFont);
  myLcd.print("suprelectronics.com", CENTER, 130); // Prints the string
  myLcd.setColor(0, 0, 0); // Sets color to white
  myLcd.setFont(UbuntuBold); // Sets font to big
  myLcd.print("Demarrage...", CENTER, 250); // Prints the string on the screen
  delay(1500);
  myLcd.setColor(0, 0, 0);
  myLcd.setFont(SmallFont);
  myLcd.print("Initialisation..   ", CENTER, 130); // Prints the string
  delay(1000);
  myLcd.print("Configuration Memoire.", CENTER, 120); // Prints the string
  delay(1000);
  myLcd.print("configuration reseau..", CENTER, 110); // Prints the string
  delay(1000);
  myLcd.print("Fin de la configuration!", CENTER, 100); // Prints the string
  delay(1000);

  myLcd.clrScr();
  myLcd.fillScr(VGA_WHITE);
  myLcd.setBackColor(VGA_WHITE);
  myLcd.drawBitmap(100, 350, 100, 92, ujn4);
  myLcd.setColor(0, 0, 0);
  myLcd.setFont(BigFont);
  myLcd.print("Complexe Scolaire", CENTER, 240);
  myLcd.setColor(0, 0, 255);
  myLcd.setFont(UbuntuBold);
  myLcd.print("UN-JOUR", CENTER, 180);
  myLcd.print("NOUVEAU", CENTER, 145);
  delay(3000);
  myLcd.drawRect(100, 350, 300, 260);
}
void show(String message) {
  if (message == " Successfull " || message == "Deja present") {
    myLcd.setColor(0, 255, 0);
    myLcd.setFont(UbuntuBold);
    myLcd.print(message, CENTER, 68);
    delay(3000);
    myLcd.print("                                          ", CENTER, 70);
    //
  }
  else {
    myLcd.setColor(255, 0, 0);
    myLcd.setFont(UbuntuBold);
    myLcd.print(message, CENTER, 70);
    delay(3000);
    myLcd.print("                                     ", CENTER, 70);
    //
  }
}
void show_details(String nom, String postnom,
                  String prenom, String classe) {
  myLcd.setColor(0, 0, 0);
  myLcd.setFont(BigFont);
  myLcd.print(nom, 40, 250);
  myLcd.print(postnom, 40, 220);
  myLcd.print(prenom, 40, 190);
  myLcd.print(classe, 40, 160);
  delay(2000);
  myLcd.print("                ", 40, 250);
  myLcd.print("                ", 40, 220);
  myLcd.print("                ", 40, 190);
  myLcd.print("                ", 40, 160);
}

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
