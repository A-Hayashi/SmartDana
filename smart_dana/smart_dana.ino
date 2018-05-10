#include "ChRt.h"
#include "PS_PAD.h"

#define PS2_DAT        12  //PD12(MISO)
#define PS2_CMD        11  //PD11(MOSI)
#define PS2_CLK        13  //PD13(SCK)
#define PS2_SEL        10  //PD10(SS)

PS_PAD PAD(PS2_SEL);

#define RC522_SDA        6  //PD6(SS)
#define RC522_RESET      5  //PD5(RESET)

static THD_WORKING_AREA(waThread1, 64);

static THD_FUNCTION(Thread1, arg) {
  PAD.init();
  (void)arg;
  while (true) {
    int deg;
    PAD.poll();
    deg = PAD.read(PS_PAD::ANALOG_RX);
    Serial.print(deg);
    Serial.print("\t");
    deg = map(deg, -128, 127, 0, 180);
    Serial.println(deg);

    chThdSleepMilliseconds(50);
  }
}

static THD_WORKING_AREA(waThread2, 64);

static THD_FUNCTION(Thread2, arg) {
  (void)arg;
  while (true) {
    Serial.println("==========1s==========");
    chThdSleepMilliseconds(1000);
  }
}

void chSetup() {
  // Start threads.
  chThdCreateStatic(waThread1, sizeof(waThread1),
                    NORMALPRIO + 2, Thread1, NULL);

  chThdCreateStatic(waThread2, sizeof(waThread2),
                    NORMALPRIO + 1, Thread2, NULL);
}

void setup() {
  pinMode(RC522_SDA, OUTPUT);
  digitalWrite(RC522_SDA, HIGH);
  Serial.begin(9600);
  chBegin(chSetup);
  while (true) {}
}


void loop() {
  chThdSleepMilliseconds(1000);
  // Print unused stack space in bytes.
  Serial.print(F(", Unused Stack: "));
  Serial.print(chUnusedThreadStack(waThread1, sizeof(waThread1)));
  Serial.print(' ');
  Serial.print(chUnusedThreadStack(waThread2, sizeof(waThread2)));
  Serial.print(' ');
  Serial.print(chUnusedMainStack());

  Serial.println();
}
