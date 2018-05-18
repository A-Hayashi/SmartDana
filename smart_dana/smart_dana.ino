#include "PS_PAD.h"
#include "SPI.h"
#include "MFRC522.h"
#include "VarSpeedServo.h"
#include "I2C.h"

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

#define SERVO1_SIGNAL  5
VarSpeedServo servo1;

#define LIGHT_PWM   3
#define LIGHT_LAMP  8
#define LIGHT_MOTOR 7

#define LEAD_SW 2

bool auth_state = false;
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
    auth_cnt = AHTH_TH;
  } else {
    Serial.println("**Acces denied**");
    auth_cnt = max(auth_cnt--, 0);
  }

END:
  {

    static bool auth_state_old = false;
    auth_state = false;
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


void setup() {
  //  pinMode(RC522_SDA, OUTPUT);
  //  digitalWrite(RC522_SDA, HIGH);
  //  pinMode(PS2_SEL, OUTPUT);
  //  digitalWrite(PS2_SEL, HIGH);

  SPI.begin();
  rfid.PCD_Init();

  pinMode(LIGHT_LAMP, OUTPUT);
  digitalWrite(LIGHT_LAMP, HIGH);
  pinMode(LIGHT_MOTOR, OUTPUT);
  digitalWrite(LIGHT_MOTOR, HIGH);
  pinMode(LIGHT_PWM, OUTPUT);
  digitalWrite(LIGHT_PWM, LOW);

  pinMode(LEAD_SW, INPUT);

  pinMode(RC522_SDA, OUTPUT);
  digitalWrite(RC522_SDA, HIGH);
  PAD.init();
  servo1.detach();
  Serial.begin(9600);
  I2C_begin(8);
}

byte pad_data[6];
byte lead_sw;
void loop() {
  static unsigned long rfid_last = 0;
  unsigned long rfid_time = 0;

  rfid_time = millis();
  if (rfid_time - rfid_last > 1000)
  {
    rfid_main();
    rfid_last = rfid_time;
  }
  PAD.poll();
//  interrupts();
  if (cServo.Number == 0) {
    servo1.write(cServo.Angle, cServo.Speed, false);
  }

  if (!servo1.isMoving()) {
    servo1.detach();
  } else {
    servo1.attach(SERVO1_SIGNAL);
  }

  if (cPatLite.Lamp) {
    digitalWrite(LIGHT_LAMP, LOW);
  } else {
    digitalWrite(LIGHT_LAMP, HIGH);
  }

  if (PAD.read(PS_PAD::PAD_RIGHT)) {
    digitalWrite(LIGHT_MOTOR, LOW);
  } else {
    digitalWrite(LIGHT_MOTOR, HIGH);
  }
  {

    byte duty = cPatLite.Speed;
    if (duty > 0) {
      digitalWrite(LIGHT_MOTOR, LOW);
    } else {
      digitalWrite(LIGHT_MOTOR, HIGH);
    }
      pad_data[0] = PAD.read(PS_PAD::BUTTONS) & 0xff;
      pad_data[1] = (PAD.read(PS_PAD::BUTTONS) >> 8) & 0xff;
      pad_data[2] = PAD.read(PS_PAD::ANALOG_RX);
      pad_data[3] = PAD.read(PS_PAD::ANALOG_RY);
      pad_data[4] = PAD.read(PS_PAD::ANALOG_LX);
      pad_data[5] = PAD.read(PS_PAD::ANALOG_LY);
      lead_sw = (~digitalRead(LEAD_SW)) & 0x01;
    TCCR2A = 0b00100011;    //比較一致でLow、BOTTOMでHighをOC2Aﾋﾟﾝへ出力 (非反転動作)
    //高速PWM動作
    TCCR2B = 0b00001010;    //高速PWM動作, clkT2S/8 (8分周)
    // TOP値指定
    OCR2A = 99;             //16MHz/(8*(1+99))=20KHz

    OCR2B = (byte)duty;
  }
   delay(100);
  //chThdSleepMilliseconds(500);
}


void I2C_RequestCbk(byte reg, byte* p_data, byte* p_size)
{
  switch (reg) {
    // Asume that the receiving end uses the same float representation
    case Req_PsPad:
      for(int i=0; i<6; i++){
        p_data[i] = pad_data[i];
      }
      *p_size = 6;
      break;
    case Req_RC522:
      p_data[0] = auth_state;
      *p_size = 1;
      break;
    case Req_Switch:
      p_data[0] = lead_sw;
      *p_size = 1;
      break;
    default:
      break;
  }
}

