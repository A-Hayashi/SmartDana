#include <MsTimer2.h>

#include "PS_PAD.h"

#define PS2_DAT        12  //PD12(MISO)
#define PS2_CMD        11  //PD11(MOSI)
#define PS2_CLK        13  //PD13(SCK)
#define PS2_SEL        10  //PD10(SS)

PS_PAD PAD(PS2_SEL);

#define RC522_SDA        6  //PD6(SS)
#define RC522_RESET      5  //PD5(RESET)



void timerFire() {
  int deg;
  PAD.poll();
  deg = PAD.read(PS_PAD::ANALOG_RX);
  Serial.print(deg);
  Serial.print("\t");
  deg = map(deg, -128, 127, 0, 180);
  Serial.println(deg);
}

void setup() {
  PAD.init();
  MsTimer2::set(100, timerFire);
  MsTimer2::start();
  pinMode(RC522_SDA, OUTPUT);
  digitalWrite(RC522_SDA, HIGH);
  Serial.begin(9600);
}


void loop() {

}
