#include "HTTP.h"

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

EthernetClient client;

void http_init(byte client_no) {
  Serial.println("========== http_init ==========");
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);

  Ethernet.begin(mac_list[client_no], ip_list[client_no]);

  Serial.print("My IP address: ");
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
    Serial.println("disconnecting.");
    client.stop();
  }

  lastConnected = client.connected();
}


static String get_json(unsigned long number, bool online)
{
  char buff[20];
  sprintf(buff, "%lu", number);

  String str;
  if (online == true)
    str = "true";
  else {
    str = "false";
  }
  
  String json = "{\n";
  json = json + "\"" + "number" + "\": " + String(buff) + ",\n";
  json = json + "\"" + "online" + "\": " + str          + "\n";
  json = json + "}\n";
  return json;
  
//  String json = "{\n";
//  json = json + "\"" + "number" + "\": \"" + "12345678" + "\",\n";
//  json = json + "\"" + "online" + "\": \"" + "1" + "\"\n";
//  json = json + "}\n";
//  return json;
}

static String get_http(String json)
{
  char buff[20];
  sprintf(buff, "%d.%d.%d.%d", server[0], server[1], server[2], server[3]);

  String str = "POST /api/TAG HTTP/1.1\n";
  str = str + "Host: ";
  str = str + String(buff) + "\n";
  str = str + "User-Agent: Arduino/uno\n";
  str = str + "Connection: close\n";
  str = str + "Content-Length: " + json.length() + "\n";
  str = str + "\n";
  str = str + json + "\n";

  return str;
}


// this method makes a HTTP connection to the server:
void httpRequest(unsigned long number, bool online) {
  if (client.connect(server, 8888)) {
    Serial.println("connecting...");

    String http = get_http(get_json(number, online));
    Serial.println(http);
    client.println(http);

  } else {
    Serial.println("connection failed");
    Serial.println("disconnecting.");
    client.stop();
  }
}
