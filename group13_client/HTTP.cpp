#include "HTTP.h"
#include "ICMPPing.h"

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

int server_port = 8888;


EthernetClient client;

SOCKET pingSocket = 0;
ICMPPing ping(pingSocket, (uint16_t)random(0, 255));

void ping_to_server()
{
  char buffer [256];

  ICMPEchoReply echoReply = ping(server, 4);
  if (echoReply.status == SUCCESS)
  {
    sprintf(buffer,
            "Reply[%d] from: %d.%d.%d.%d: bytes=%d time=%ldms TTL=%d",
            echoReply.data.seq,
            echoReply.addr[0],
            echoReply.addr[1],
            echoReply.addr[2],
            echoReply.addr[3],
            REQ_DATASIZE,
            millis() - echoReply.data.time,
            echoReply.ttl);
  }
  else
  {
    sprintf(buffer, "Echo request failed; %d", echoReply.status);
  }
  Serial.println(buffer);
}

void http_init(byte client_no) {
  Serial.println(F("========== http_init =========="));
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);

  Ethernet.begin(mac_list[client_no], ip_list[client_no]);

  Serial.print(F("My IP address: "));
  Serial.println(Ethernet.localIP());
}


void http_main()
{
  static boolean received = false;
  static boolean lastConnected = false;

  while (client.available()) {
    char c = client.read();
    Serial.write(c);
    received = true;
  }

  if (received == true) {
    client.stop();
    received = false;
  }

  if (!client.connected() && lastConnected) {
    Serial.println();
    Serial.println(F("disconnecting."));
    client.stop();
  }

  lastConnected = client.connected();
}

static client_print_debug(String str)
{
  client.print(str);
  Serial.print(str);
}

static void send_http(unsigned long number, bool online)
{
  char buff[30];
  char buff2[10];
  sprintf(buff, "%d.%d.%d.%d:%d", server[0], server[1], server[2], server[3], server_port);
  sprintf(buff2, "%lu", number);

  String str;
  if (online == true)
    str = "true";
  else {
    str = "false";
  }

  String json = "{";
  json = json + "\"" + "number" + "\":" + String(buff2) + ",";
  json = json + "\"" + "online" + "\":" + str;
  json = json + "}\r\n";

  client_print_debug(F("PUT /api/TAG HTTP/1.1\r\n"));
  client_print_debug(F("Accept: application/json\r\n"));
  client_print_debug(F("Content-Type: application/json; charset=utf-8\r\n"));

  client_print_debug(F("Host: "));
  client_print_debug( String(buff));
  client_print_debug(F("\r\n"));

  client_print_debug(F("Content-Length: "));
  client_print_debug(String(json.length()));
  client_print_debug(F("\r\n"));

  client_print_debug(F("Expect: 100-continue\r\n"));
  client_print_debug(F("Connection: Keep-Alive\r\n"));
  client_print_debug(F("\r\n"));
  client_print_debug(json);
}


static void HexDump(String str)
{
  Serial.print(F("Total: "));
  Serial.print(str.length());
  Serial.println(F(" bytes"));

  for (int i = 0; i < str.length(); i++) {
    if (i % 16 == 0) {
      if (i < 0x10) {
        Serial.print(F("000"));
      } else if (i < 0x100) {
        Serial.print(F("00"));
      } else if (i < 0x1000) {
        Serial.print(F("0"));
      }
      Serial.print(i, HEX);
      Serial.print(F("   "));
    }
    if (str[i] < 0x10) Serial.print(F("0"));
    Serial.print(str[i], HEX);
    Serial.print(" ");
    if ((i + 1) % 16 == 0) Serial.print(F("\n"));
  };
  Serial.print(F("\n\n"));
}


// this method makes a HTTP connection to the server:
void httpRequest(unsigned long number, bool online) {
  Serial.println(F("httpRequest"));

  int result = client.connect(server, server_port);
  Serial.print(F("connect ressult: "));
  Serial.println(result);
  if (result) {
    Serial.println(F("connecting..."));
    send_http(number, online);
  } else {
    Serial.println(F("connection failed"));
    Serial.println(F("disconnecting."));
    client.stop();
  }
}
