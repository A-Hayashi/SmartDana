#include <SPI.h>
#include "MFRC522.h"

typedef enum _state_t
{
  STATE_OFFLINE,
  STATE_ONLINE,
  STATE_UNDECIDED,
} state_t;


void rfid_set_euid(unsigned long _euid);
void rfid_set_timeout(unsigned long _timeout);
void rfid_init(void (*cbk_uid)(unsigned long), void (*cbk_state)(unsigned long, state_t));
void rfid_main();

