#include "EEP.h"

void eeprom_write(EEPSTRUCT* s)
{
  byte* p = (byte*)s;
  for (int j = 0; j < sizeof(EEPSTRUCT); j++) {
    EEPROM.write(j, *p);
    p++;
  }
}

void eeprom_read(EEPSTRUCT* s)
{
  byte* p = (byte*)s;
  for (int j = 0; j < sizeof(EEPSTRUCT); j++) {
    byte b = EEPROM.read(j);
    *p = b;
    p++;
  }
  Serial.println("========== eeprom_read ==========");
  Serial.print("TIMEOUT:");
  Serial.println(s->timeout);
  Serial.print("CLIENT_NO:");
  Serial.println(s->client_no);
  Serial.print("EXPECTED_UID:");
  Serial.println(s->euid, HEX);
}
