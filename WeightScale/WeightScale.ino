#include <ESP8266WiFi.h>

#define PIN 4
#define LED 16
#define  SW 5

#define TIMEOUT_US 30000

const char* ssid = "W04_78625678F45E";//書き換えてください
const char* password = "6dqbfjgfyb6gi22";//書き換えてください

char thingSpeakAddress[] = "api.thingspeak.com";
String thingtweetAPIKey = "V9RTFMB7G0M6WEEI";//書き換えてください

char hostIP[] = "192.168.100.107";
int  hostPort = 80;

WiFiClient client;

/*
   39bit 値を保存するため、uint32_t の変数を2つ使っている。
   Arduino 環境における uint64_t は罠が多いため避ける。
*/
int read_bits(uint32_t *bh, uint32_t *bl) {
  int count = 0;
  uint64_t ret = 0;

  while (true) {
    unsigned long pulse = pulseIn(PIN, HIGH, TIMEOUT_US);
    if (pulse == 0) break;

    uint32_t bit = pulse < 750 ? 1 : 0;
    if (count < 32) {
      bitWrite(*bh, 31 - count, bit);
    } else if (count < 64) {
      bitWrite(*bl, 63 - count, bit);
    }
    ++count;
  }
  return count;
}

void setup() {
  pinMode(PIN, INPUT);
  pinMode(SW, INPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  Serial.begin(115200);

  Serial.println("Booting...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed. Rebooting...");
    delay(5000);
    ESP.restart();
  }
  Serial.println("Connected to wifi");
}

void loop() {
  uint32_t bh = 0;
  uint32_t bl = 0;

  int count = read_bits(&bh, &bl);
  static byte sw_old = HIGH;
  byte sw_now = HIGH;
  /*
    シリアル通信に一定の時間がかかるため、表示処理の間に幾つかの信号を取り逃してしまう。
    また、不完全な信号を受信する可能性もある。
    今回は、目的の 39 bit を読めたときだけ表示することとする。
  */
  sw_now = digitalRead(SW);
  if (sw_old != sw_now) {
    Serial.print("SW: ");
    Serial.println(sw_now);
    String tcpStr = "SW:" + String(sw_now) + "\n";
    sendSocket(tcpStr);
    //String url = "?sw=" + String(sw_now);
    //send_to_lychee(url);
  }
  sw_old = sw_now;

  if (count == 39) {
    byte stable = (bh >> 18) & 0x03;

    Serial.print(stable, BIN);
    Serial.print(' ');
    Serial.print(bh, BIN);
    Serial.print(' ');
    Serial.println(bl >> 25, BIN);
    if (stable == 0x03) {
      float weight = (bh & 0xffff) / (float)10;
      int weight10 = bh & 0xffff;
      Serial.print("Weight: ");
      Serial.println(weight, 1);
      String tcpStr = "WEIGHT:" + String(weight10) + "\n";
      sendSocket(tcpStr);
      String twStr = "Tweeting from ESP8266. my weight: " + String(weight) + " kg";
      updateTwitterStatus(twStr);
      digitalWrite(LED, LOW);
      delay(5000);
    } else {
      digitalWrite(LED, HIGH);
    }
  } else {
    digitalWrite(LED, HIGH);
  }
}

void sendSocket(String str)
{
  if (client.connect(hostIP, hostPort))
  {
    Serial.print("Connected:");
    Serial.println(hostIP);
    Serial.println("Posting: " + str);

    //送信
    client.print(str);

    //応答受信
    client.setTimeout(1000);
    do {
      String line = client.readStringUntil('\n');
      Serial.print(line);
    } while (client.available() != 0);  //残りがあるときはさらに受信のためループ
    Serial.println();
  }
  else
  {
    Serial.println("Connection failed.");
  }
}


void updateTwitterStatus(String tsData)
{
  if (client.connect(thingSpeakAddress, 80))
  {
    Serial.println("Connected to ThingSpeak.");
    Serial.println("Posting: " + tsData);

    // Create HTTP POST Data
    tsData = "api_key=" + thingtweetAPIKey + "&status=" + tsData;

    client.print("POST /apps/thingtweet/1/statuses/update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(tsData.length());
    client.print("\n\n");

    client.print(tsData);
    client.stop();
  }
  else
  {
    Serial.println("Connection failed.");
  }
}





void send_to_lychee(String url)
{
  if (client.connect(hostIP, hostPort))
  {
    Serial.println("Connected to GR-LYCHEE.");
    Serial.print("Requesting URL: ");
    Serial.println(url);

    Serial.print("GET /secret/" + url + " HTTP/1.1\r\n");
    Serial.print("Host: " + String(hostIP) + "\r\n");
    Serial.print("Connection: Close\r\n");
    Serial.print("\r\n");
    
    client.print("GET /secret/" + url + " HTTP/1.1\r\n");
    client.print("Host: " + String(hostIP) + "\r\n");
    client.print("Connection: Close\r\n");
    client.print("\r\n");
    
    client.setTimeout(1000);
    do {
      String line = client.readStringUntil('\n');
      Serial.println(line);
    } while (client.available() != 0);  //残りがあるときはさらに受信のためループ
    Serial.println();
    
    client.stop();
  }else{
    Serial.println("Connection failed.");
  }
}
