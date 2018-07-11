#include <SPI.h>
#include <Ethernet.h>
#include <EEPROM.h>

typedef struct _EEPSTRUCT {
  byte client_no;
} EEPSTRUCT;


EEPSTRUCT s2;
byte mac_list[10][6] = {
  {0x5d, 0x06, 0x92, 0x90, 0x1e, 0xe7},
  {0x3a, 0xc0, 0x3c, 0xda, 0xe8, 0xc8},
  {0x72, 0x0a, 0x2a, 0x6a, 0x07, 0x69},
  {0x2d, 0xe3, 0x42, 0x66, 0x56, 0xa4},
  {0x31, 0x77, 0x46, 0x07, 0xe6, 0x66},
  {0x8d, 0x3d, 0x17, 0x03, 0x64, 0xad},
  {0x0a, 0xb2, 0xa6, 0xb6, 0x55, 0x47},
  {0xd8, 0xae, 0x30, 0x45, 0x9e, 0x23},
  {0xe2, 0x62, 0xc9, 0x63, 0x1c, 0x63},
  {0x82, 0xb9, 0xcd, 0xa9, 0x2d, 0x96},
};

byte ip_list[10][4] = {
  {192, 168, 0, 10},
  {192, 168, 0, 11},
  {192, 168, 0, 12},
  {192, 168, 0, 13},
  {192, 168, 0, 14},
  {192, 168, 0, 15},
  {192, 168, 0, 16},
  {192, 168, 0, 17},
  {192, 168, 0, 18},
  {192, 168, 0, 19},
};

byte server[] = {192, 168, 0, 1};

byte client_no = 0;
unsigned long lastConnectionTime = 0;
const unsigned long postingInterval = 10L * 1000L;

EthernetClient client;

void software_reset() {
  asm volatile ("  jmp 0");
}


void cmd()
{
  bool eeprom_write = false;
  if ( Serial.available() > 0 ) {
    String str = Serial.readStringUntil('\r');

    Serial.println();
    if ( str.startsWith("cno:") == true) {
      int cno = str.substring(4).toInt();
      if (0 <= cno && cno <= 9) {
        Serial.print("> OK : client_no:");
        Serial.println(cno);
        s2.client_no = cno;
        eeprom_write = true;
      } else {
        Serial.print("> NG : client_no:");
        Serial.println(cno);
      }
    }
    else if ( str == "off" ) {
      Serial.println("> ok : LED13 turn off");
    }
    else {
      Serial.print("> error [");
      Serial.print(str);
      Serial.println("]");
    }
  }

  if (eeprom_write == true) {
    eeprom_write = false;
    byte* p = (byte*) &s2;
    for (int j = 0; j < sizeof(EEPSTRUCT); j++) {
      EEPROM.write(j, *p);
      p++;
    }
    delay(2000);
    software_reset();
  }
}


void setup() {
  byte* p2 = (byte*) &s2;
  for (int j = 0; j < sizeof(EEPSTRUCT); j++) {
    byte b = EEPROM.read(j);
    *p2 = b;
    p2++;
  }
  client_no = s2.client_no;

  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);

  Serial.begin(9600);
  while (!Serial);

  Ethernet.begin(mac_list[client_no], ip_list[client_no]);

  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
}

void loop()
{
  cmd();
  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  if (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  // if ten seconds have passed since your last connection,
  // then connect again and send data:
  if (millis() - lastConnectionTime > postingInterval) {
    httpRequest();
  }
}



// this method makes a HTTP connection to the server:
void httpRequest() {
  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();

  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("connecting...");
    // send the HTTP GET request:
    client.println("GET /latest.txt HTTP/1.1");
    client.println("Host: www.arduino.cc");
    client.println("User-Agent: arduino-ethernet");
    client.println("Connection: close");
    client.println();

    // note the time that the connection was made:
    lastConnectionTime = millis();
  } else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }
}

