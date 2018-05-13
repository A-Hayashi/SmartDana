#define PIN D2

#define TIMEOUT_US 30000

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
}

void loop() {
  uint32_t bh = 0;
  uint32_t bl = 0;

  int count = read_bits(&bh, &bl);
 // Serial.println(count);
  if (count == 39) {
    Serial.println(bh & 0xFFFF);
  }
}
