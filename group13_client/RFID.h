#include <SPI.h>
#include "MFRC522.h"

typedef enum _state_t
{
  STATE_ONLINE,
  STATE_OFFLINE,
  STATE_UNDECIDED,
} state_t;


void rfid_set_euid(unsigned long _euid);
void rfid_set_timeout(unsigned long _timeout);
void rfid_init(void (*cbk_uid)(unsigned long), void (*cbk_state)(state_t));
void rfid_main();

