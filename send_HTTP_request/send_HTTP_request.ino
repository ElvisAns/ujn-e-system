#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <SPI.h>

ESP8266WiFiMulti WiFiMulti;
#define RST_PIN 0  // RST-PIN for RC522 - RFID - SPI - Modul GPIO15 
#define SS_PIN  2  // SDA-PIN for RC522 - RFID - SPI - Modul GPIO2
#define BUZZER 15
#define RED_LIGHT 16
#define YELLOW_LIGHT 4
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance


int frequency = 2000;
int frequency2 = 1000;
char *UID;
String data, tag;

void setup() {
  const uint16_t port = 80;
  const char * host = "192.168.43.23/"; // ip or dns
  pinMode(BUZZER, OUTPUT);
  pinMode(RED_LIGHT, OUTPUT);
  pinMode(YELLOW_LIGHT, OUTPUT);
  digitalWrite(BUZZER, LOW);
  digitalWrite(YELLOW_LIGHT, LOW);
  digitalWrite(RED_LIGHT, HIGH);
  Serial.begin(9600);
  SPI.begin();           // Init SPI bus
  mfrc522.PCD_Init();    // Init MFRC522
  delay(30);

  // We start by connecting to a WiFi network
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("greenzone", "12345678e");
  //Serial.println();
  //Serial.println();
  //Serial.print("Wait for WiFi... ");
  unsigned long m = millis();
  Serial.println();
  Serial.print("network-off");
  while (WiFiMulti.run() != WL_CONNECTED || millis() < 10000) {
    delay(100);
    if ((millis() - m) > 800) {
      m = millis();
      Serial.println();
      if (WiFiMulti.run() != WL_CONNECTED)Serial.print("network-off");
      else Serial.print("network-connected");
    }
  }
  Serial.println();
  m = millis();
  while ((millis() - m) > 5000) {
    delay(1000);
    Serial.println();
    Serial.print("network-connected");
  }

  delay(1000);
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");

  Serial.println(WiFi.localIP());
  Serial.print("connected to ");
  Serial.println(host);
}
int hasSaid = false;
unsigned long m;

void (*reset_func)(void) = 0;

void loop() {

  if (WiFiMulti.run() != WL_CONNECTED) {
    delay(100);
    reset_func();
  }
 
  digitalWrite(RED_LIGHT, LOW);
  const uint16_t port = 80;
  const char * host = "192.168.43.23/"; // ip or dns

  HTTPClient http;
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    delay(20);
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    delay(20);
    return;
  }
  for (byte i = 0; i <  mfrc522.uid.size; i++) {
    tag += mfrc522.uid.uidByte[i];
  }
  while (tag.length() <= 13) {
    tag = tag + '0';
  }
  data += "?status=" + tag ;
  String savetag = tag;
  String Link = "http://192.168.43.23/ujn/process.php" + data;
  http.begin(Link);
  int httpCode = http.GET();
  //Serial.println(Link);
  //Serial.println(httpCode);
  tag = "";
  data = "";
  if (httpCode == 200) {
    String payload = http.getString();// gives us the message received by the GET Request
    payload.trim();
    //Serial.println("Message : ");
    Serial.print(payload);
    //Serial.println();
    if (payload.length() >= 30) {
      tone(BUZZER, frequency);
      digitalWrite(RED_LIGHT, LOW);
      digitalWrite(YELLOW_LIGHT, HIGH);
      delay(600);
      noTone(BUZZER);
      digitalWrite(RED_LIGHT, LOW);
      digitalWrite(YELLOW_LIGHT, LOW);
    }
    else {
      if (payload == "already-read") {
        digitalWrite(YELLOW_LIGHT, HIGH);
        delay(200);
        digitalWrite(YELLOW_LIGHT, LOW);
        delay(100);
        digitalWrite(YELLOW_LIGHT, HIGH);
        delay(200);
        digitalWrite(YELLOW_LIGHT, LOW);
      }
      else if (payload == "invalid-att") {
        invalid_fee();
      }
      else if (payload == "not-found") {
        invalid();
      }
      else if (payload == "invalid-time") {
        invalid_fee();
      }
      else if (payload == "deactivated") {
        Serial.print("deactivated");
        invalid();
      }
      else { //for invalid-time, invalid-id,...
        Serial.print("invalid");
        invalid();
      }
    }
    http.end();
  }
  else {
    no_response_found();
  }
  delay(500);

}
void no_response_found() {
  digitalWrite(RED_LIGHT, HIGH);
  digitalWrite(YELLOW_LIGHT, HIGH);
  tone(BUZZER, frequency);
  delay(3000);
  noTone(BUZZER);
  digitalWrite(RED_LIGHT, LOW);
  digitalWrite(YELLOW_LIGHT, LOW);
  Serial.print("system-down");

}
void invalid() {
  tone(BUZZER, frequency);
  digitalWrite(RED_LIGHT, LOW);
  digitalWrite(YELLOW_LIGHT, HIGH);
  delay(100);
  noTone(BUZZER);
  digitalWrite(RED_LIGHT, 1);
  digitalWrite(YELLOW_LIGHT, 0);
  delay(150);
  tone(BUZZER, frequency);
  digitalWrite(RED_LIGHT, LOW);
  digitalWrite(YELLOW_LIGHT, HIGH);
  delay(100);
  noTone(BUZZER);
  digitalWrite(RED_LIGHT, 1);
  digitalWrite(YELLOW_LIGHT, 0);
  delay(150);

  tone(BUZZER, frequency);
  digitalWrite(RED_LIGHT, LOW);
  digitalWrite(YELLOW_LIGHT, HIGH);
  delay(100);
  noTone(BUZZER);
  digitalWrite(RED_LIGHT, 1);
  digitalWrite(YELLOW_LIGHT, 0);
  delay(150);
  tone(BUZZER, frequency);
  digitalWrite(RED_LIGHT, LOW);
  digitalWrite(YELLOW_LIGHT, HIGH);
  delay(100);
  noTone(BUZZER);
  digitalWrite(RED_LIGHT, 1);
  digitalWrite(YELLOW_LIGHT, 0);
  delay(150);
}
void invalid_fee() {
  digitalWrite(RED_LIGHT, HIGH);
  tone(BUZZER, frequency2);
  delay(1000);
  noTone(BUZZER);
  digitalWrite(RED_LIGHT, HIGH);
}
