#include "RFID.h"

#define RST_PIN 8
#define SS_PIN 9

static void ShowReaderDetails();

static MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance


static void (*cbk1)(unsigned long);
static void (*cbk2)(unsigned long, state_t);

static unsigned long euid = 0xAAAAAAAA;
static unsigned long start;
static unsigned long timeout = 3000;
static state_t state = STATE_OFFLINE;
static state_t state_old = STATE_UNDECIDED;

void rfid_set_euid(unsigned long _euid) {
  euid = _euid;
}

void rfid_set_timeout(unsigned long _timeout) {
  timeout = _timeout;
}

void rfid_init(void (*cbk_uid)(unsigned long), void (*cbk_state)(unsigned long, state_t)) {
  Serial.println("========== rfid_init ==========");

  cbk1 = cbk_uid;
  cbk2 = cbk_state;

  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);

  SPI.begin(); // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522
  ShowReaderDetails(); // Show details of PCD - MFRC522 Card Reader details
  Serial.println(F("Scan PICC to see UID, type, and data blocks..."));

  start = millis();
}

void rfid_main() {
  unsigned long uid = 0x00000000;
  bool tag_detected = false;

  // Look for new cards
  if ( mfrc522.PICC_IsNewCardPresent()) {
    // Select one of the cards
    if ( mfrc522.PICC_ReadCardSerial()) {
      // Dump debug info about the card; PICC_HaltA() is automatically called
      uid = ((unsigned long)mfrc522.uid.uidByte[3] << 24)
            + ((unsigned long)mfrc522.uid.uidByte[2] << 16)
            + ((unsigned long)mfrc522.uid.uidByte[1] << 8)
            + (unsigned long)mfrc522.uid.uidByte[0];
      tag_detected = true;
      if (cbk1 != NULL) {
        (*cbk1)(uid);
      }
    }
  }

  if (
    tag_detected == true
    && ((uid == euid) || ( euid == 0xFFFFFFFF))
  ) {
    start = millis();
    state = STATE_ONLINE;
  }

  if (millis() - start > timeout) { //OVFの考慮が必要
    state = STATE_OFFLINE;
  }

  if (state_old != state) {
    static unsigned long uid_old;
    if (state == STATE_ONLINE) {
      uid_old = uid;
    }
    if (cbk2 != NULL) {
      (*cbk2)(uid_old, state);
    }
  }

  state_old = state;
}

static void ShowReaderDetails() {
  // Get the MFRC522 software version
  byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
  Serial.print(F("MFRC522 Software Version: 0x"));
  Serial.print(v, HEX);
  if (v == 0x91)
    Serial.print(F(" = v1.0"));
  else if (v == 0x92)
    Serial.print(F(" = v2.0"));
  else
    Serial.print(F(" (unknown)"));
  Serial.println("");
  // When 0x00 or 0xFF is returned, communication probably failed
  if ((v == 0x00) || (v == 0xFF)) {
    Serial.println(F("WARNING: Communication failure, is the MFRC522 properly connected?"));
  }
}

