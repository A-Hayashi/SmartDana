#include "ChRt.h"
#include "PS_PAD.h"
#include "SPI.h"
#include "MFRC522.h"

#define A5 19
#define A4 18
#define A3 17
#define A2 16
#define A1 15
#define A0 14


#define PS2_SEL        A3
PS_PAD PAD(PS2_SEL);

#define RC522_SDA        A2
#define RC522_RESET      A1
MFRC522 rfid(RC522_SDA, RC522_RESET);

BSEMAPHORE_DECL(SemSPI, false);


static THD_WORKING_AREA(waThread1, 64);

static THD_FUNCTION(Thread1, arg) {
  PAD.init();
  (void)arg;
  while (true) {
    chBSemWait(&SemSPI);
    pad_main();
    chBSemSignal(&SemSPI);
    chThdSleepMilliseconds(50);
  }
}


void pad_main()
{
  int deg;
  PAD.poll();

  deg = PAD.read(PS_PAD::ANALOG_RX);
  Serial.print(deg);
  Serial.print("\t");
  deg = map(deg, -128, 127, 0, 180);
  Serial.println(deg);
  Serial.println();
}


#define AHTH_TH 10
void rfid_main()
{
  const char expected_uid[] = {0x61, 0x20, 0xC4, 0x30};
  static int auth_cnt = AHTH_TH;

  if (!rfid.PICC_IsNewCardPresent()) {
    Serial.println(F("New card is not present"));
    auth_cnt = max(auth_cnt--, 0);
    goto END;
  }

  if (!rfid.PICC_ReadCardSerial()) {
    Serial.println(F("Card cannot be read"));
    goto END;
  }

  {
    MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
    if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
        piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
        piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
      Serial.println(F("Your tag is not of type MIFARE Classic."));
      auth_cnt = max(auth_cnt--, 0);
      goto END;
    }
  }

  Serial.print("Tap card key: ");
  for (byte i = 0; i < 4; i++) {
    Serial.print(rfid.uid.uidByte[i], HEX);
    if (i < 3) {
      Serial.print(":");
    } else {
      Serial.println("");
    }
  }

  if (memcmp(expected_uid, rfid.uid.uidByte, sizeof(expected_uid)) == 0) {
    Serial.println("**Authorised acces**");
    auth_cnt = AHTH_TH;
  } else {
    Serial.println("**Acces denied**");
    auth_cnt = max(auth_cnt--, 0);
  }

END:
  {
    bool auth_state = false;
    static bool auth_state_old = false;
    Serial.println(auth_cnt);

    if (auth_cnt > 0) {
      auth_state = true;
    } else {
      auth_state = false;
    }

    if (auth_state_old != auth_state) {
      if (auth_state == true) {
        Serial.println("false to true");
      } else {
        Serial.println("true to false");
      }
    }
    auth_state_old = auth_state;
  }
}


static THD_WORKING_AREA(waThread2, 256);
static THD_FUNCTION(Thread2, arg)
{
  SPI.begin();
  rfid.PCD_Init();
  (void)arg;
  while (true) {
    chBSemWait(&SemSPI);
    rfid_main();
    chBSemSignal(&SemSPI);
    chThdSleepMilliseconds(1000);
  }
}

void chSetup() {
  //   Start threads.
  chThdCreateStatic(waThread1, sizeof(waThread1),
                    NORMALPRIO + 2, Thread1, NULL);

  chThdCreateStatic(waThread2, sizeof(waThread2),
                    NORMALPRIO + 1, Thread2, NULL);
}


void setup() {
  pinMode(RC522_SDA, OUTPUT);
  digitalWrite(RC522_SDA, HIGH);
  pinMode(PS2_SEL, OUTPUT);
  digitalWrite(PS2_SEL, HIGH);
  Serial.begin(9600);
  chBegin(chSetup);
  while (true) {}
}


void loop() {
  chThdSleepMilliseconds(1000);
  // Print unused stack space in bytes.
  //  Serial.print(F(", Unused Stack: "));
  //  Serial.print(chUnusedThreadStack(waThread1, sizeof(waThread1)));
  //  Serial.print(' ');
  //  Serial.print(chUnusedThreadStack(waThread2, sizeof(waThread2)));
  //  Serial.print(' ');
  //  Serial.print(chUnusedMainStack());

  Serial.println();
}
