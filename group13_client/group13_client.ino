#include "RFID.h"
#include "HTTP.h"
#include "EEP.h"

EEPSTRUCT eep;
bool uidset_flag = false;
bool write_flag = false;

static void cmd_main();

extern char *__bss_end;
extern uint8_t  __stack;
#define STACK_CANARY_VAL 0xfd

void StackMarginWaterMark(void) {
  const uint8_t *p = (uint8_t *)&__bss_end;
  uint16_t c = 0;

  while (*p == STACK_CANARY_VAL && p <= (uint8_t *)&__stack) {
    p++;
    c++;
  }
  Serial.print(F("Stack remain: "));
  Serial.println(c);
}

void StackFill(void) {
  uint8_t *p = (uint8_t *)&__bss_end;
  Serial.print(F("bss_end: "));
  Serial.println((unsigned long)p, HEX);
  Serial.print(F("SP: "));
  Serial.println(SP, HEX);
  Serial.print(F("Stack Size: "));
  Serial.println(SP - (unsigned long)p);

  while (p <= SP)
    *p++ = STACK_CANARY_VAL;
}


static void notify_uid(unsigned long uid)
{
  if (uidset_flag == true) {
    uidset_flag = false;
    eep.euid = uid;
    write_flag = true;
    Serial.println(uid, HEX);
  }
}

static void notify_state(unsigned long uid, state_t state)
{
  if (state == STATE_ONLINE) {
    Serial.println(F("RFID:STATE_ONLINE"));
  } else if (state == STATE_OFFLINE) {
    Serial.println(F("RFID:STATE_OFFLINE"));
  } else {
    Serial.println(F("RFID:STATE_UNDECIDED"));
  }
  httpRequest(uid, state);
}

void setup() {
  Serial.begin(115200); // Initialize serial communications with the PC
  while (!Serial); // Do nothing if no serial port is opened (added for Arduinos based on ATMEG
  eeprom_read(&eep);

  rfid_set_euid(eep.euid);
  rfid_set_timeout(eep.timeout);
  rfid_init(&notify_uid, &notify_state);
  http_init(eep.client_no);

  StackFill();
  StackMarginWaterMark();
}




void loop()
{
  // StackMarginWaterMark();

  cmd_main();
  http_main();
  rfid_main();
}


static void cmd_main()
{

  if ( Serial.available() > 0 ) {
    String str = Serial.readStringUntil('\r');
    Serial.println();
    if ( str.startsWith("cno:") == true) {
      int cno = str.substring(4).toInt();
      if (0 <= cno && cno <= 9) {
        Serial.print(F("> OK : client_no:"));
        Serial.println(cno);
        eep.client_no = cno;
        write_flag = true;
      } else {
        Serial.print(F("> NG : client_no:"));
        Serial.println(cno);
      }
    } else if ( str == "uidset" ) {
      Serial.print(F("> PENDING : Please set RFID tag:"));
      uidset_flag = true;
    } else if ( str.startsWith("timeout:") == true) {
      unsigned long timeout = str.substring(8).toInt();
      Serial.print(F("> OK : timeout:"));
      Serial.print(timeout);
      Serial.println(F(" [ms]"));
      eep.timeout = timeout;
      write_flag = true;
    } else if ( str == "anyuid" ) {
      Serial.println(F("> OK : anyuid:"));
      eep.euid = 0xFFFFFFFF;
      write_flag = true;
    } else if ( str == "ping" ) {
      Serial.println(F("> OK : ping:"));
      for (int i = 0; i < 4; i++) {
        ping_to_server();
        delay(500);
      }
    } else if ( str == "hr" ) {
      Serial.println(F("> OK : http request:"));
      httpRequest(100, 1);
    } else if ( str == "stack" ) {
      Serial.println(F("> OK : stack"));
      StackMarginWaterMark();
    }
    else {
      Serial.print(F("> error ["));
      Serial.print(str);
      Serial.println(F("]"));
    }
  }

  if (write_flag == true) {
    write_flag = false;
    eeprom_write(&eep);
    delay(2000);
    software_reset();
  }
}

static void software_reset() {
  asm volatile ("  jmp 0");
}
