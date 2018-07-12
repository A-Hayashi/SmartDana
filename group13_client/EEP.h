#include <Arduino.h>
#include <EEPROM.h>

typedef struct _EEPSTRUCT {
  byte client_no;
  unsigned long euid;
  unsigned long timeout;
} EEPSTRUCT;

void eeprom_write(EEPSTRUCT* s);
void eeprom_read(EEPSTRUCT* s);
