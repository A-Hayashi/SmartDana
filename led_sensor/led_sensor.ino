#define LED_P 7
#define LED_M A0

typedef enum {
  LED_CHARGE,
  LED_INPUT,
  LED_ON,
  LED_OFF,
} led_state;

long ma(long input) {

  long total1 =  input;
  long total1_ave = 0;
  long sum = 0;

#define AVERAGE_CNT 10

  static int cnt = 0;
  static long temp[AVERAGE_CNT] = {0};

  if (cnt == AVERAGE_CNT) {
    cnt = 0;
  }
  temp[cnt] = total1;
  cnt++;
  for (int i = 0; i < AVERAGE_CNT; i++) {
    sum += temp[i];
  }
  total1_ave = sum / AVERAGE_CNT;
  return total1_ave;
}



void led_control(led_state state)
{
  switch (state) {
    case LED_CHARGE:
      pinMode(LED_P, OUTPUT);
      pinMode(LED_M, OUTPUT);
      digitalWrite(LED_P, LOW);
      digitalWrite(LED_M, HIGH);
      break;
    case LED_INPUT:
      pinMode(LED_P, INPUT);
      pinMode(LED_M, OUTPUT);
      //digitalWrite(LED_P, LOW);
      digitalWrite(LED_M, HIGH);
      break;
    case LED_ON:
      digitalWrite(LED_P, HIGH);
      digitalWrite(LED_M, LOW);
      pinMode(LED_P, OUTPUT);
      pinMode(LED_M, OUTPUT);
      break;
    case LED_OFF:
      digitalWrite(LED_P, LOW);
      digitalWrite(LED_M, LOW);
      pinMode(LED_P, OUTPUT);
      pinMode(LED_M, OUTPUT);
      break;
  }
}

unsigned long pulse_measure(byte pin, bool level)
{
  unsigned long time_s = micros();
  while (digitalRead(pin) != level);
  unsigned long time_e = micros();

  return time_e - time_s;
}

unsigned long pulse_measure2(byte pin, unsigned long th)
{
  unsigned long time_s = micros();
  unsigned long ave = ma(analogRead(pin));

  while (analogRead(pin) < th) {
    //    Serial.println(analogRead(pin));
  }
  unsigned long time_e = micros();

  return time_e - time_s;
}

long period;

void setup() {
  led_control(LED_CHARGE);
  Serial.begin(9600);
  period = millis();
}

void loop() {
  if (millis() - period > 50) {
    period = millis();

    led_control(LED_CHARGE);
    delayMicroseconds(10000);

    led_control(LED_INPUT);
    unsigned long syoudo = pulse_measure(LED_P, HIGH);
    //unsigned long syoudo = pulse_measure2(LED_P, 600);
    unsigned long syoudo_ave = ma(syoudo);

    Serial.print(syoudo);
    Serial.print(",");
    Serial.print(syoudo_ave);
    Serial.println("");

    if (syoudo_ave  > 25000) {
      led_control(LED_OFF);
    } else if (syoudo_ave  < 16000) {
      led_control(LED_ON);
    }
  }
}

