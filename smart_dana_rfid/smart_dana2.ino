#include "SPI.h"
#include "MFRC522.h"

#define SS_PIN 6
#define RST_PIN 5

MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("I am waiting for card...");
}


void loop() {
  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  if ( !rfid.PICC_ReadCardSerial()) {
    return;
  }

  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
      piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
      piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }
  String strID = "";
  for (byte i = 0; i < 4; i++) {
    strID +=
      (rfid.uid.uidByte[i] < 0x10 ? "0" : "") +
      String(rfid.uid.uidByte[i], HEX) +
      (i != 3 ? ":" : "");
  }

  strID.toUpperCase();
  Serial.print("Tap card key: ");
  Serial.println(strID);

  if (strID.indexOf("9D:16:DE:73") >= 0) {  //put your own tap card key;
    Serial.println("********************");
    Serial.println("**Authorised acces**");
    Serial.println("********************");
    return;
  }
  else {
    Serial.println("****************");
    Serial.println("**Acces denied**");
    Serial.println("****************");
    return;
  }
}


