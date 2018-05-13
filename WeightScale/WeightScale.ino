#include <ESP8266WiFi.h>

#define PIN 4
#define TIMEOUT_US 30000

const char* ssid = "W04_78625678F45E";//書き換えてください
const char* password = "6dqbfjgfyb6gi22";//書き換えてください

char thingSpeakAddress[] = "api.thingspeak.com";
String thingtweetAPIKey = "V9RTFMB7G0M6WEEI";//書き換えてください

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

  /*
    シリアル通信に一定の時間がかかるため、表示処理の間に幾つかの信号を取り逃してしまう。
    また、不完全な信号を受信する可能性もある。
    今回は、目的の 39 bit を読めたときだけ表示することとする。
  */
  if (count == 39) {
    byte stable = (bh >> 18) & 0x03;

    Serial.print(stable, BIN);
    Serial.print(' ');
    Serial.print(bh, BIN);
    Serial.print(' ');
    Serial.println(bl >> 25, BIN);
    if (stable == 0x03) {
      float weight = (bh & 0xffff) / (float)10;
      Serial.print("Weight: ");
      Serial.println(weight, 1);
      String twStr = "Tweeting from ESP8266. my weight: " + String(weight) + " kg";
      updateTwitterStatus(twStr);
      delay(30000);
    }
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
