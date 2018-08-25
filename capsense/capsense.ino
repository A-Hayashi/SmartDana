#include "CapSense.h"

/*
   CapitiveSense Library Demo Sketch
   Paul Badger 2008
   Uses a high value resistor e.g. 10M between send pin and receive pin
   Resistor effects sensitivity, experiment with values, 50K - 50M. Larger resistor values yield larger sensor values.
   Receive pin is the sensor pin - try different amounts of foil/metal on this pin
*/

CapSense   cs_4_2 = CapSense(4, 2);       // 10M resistor between pins 4 & 2, pin 2 is sensor pin, add a wire and or foil if desired

#define LED 6


void setup()
{
  cs_4_2.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate on channel 1 - just as an example
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
}


#define AVERAGE_CNT 10

void loop()
{
  long start = millis();
  long total1 =  cs_4_2.capSense(30);

  long total1_ave = 0;
  long sum = 0;

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


  //  Serial.print(millis() - start);        // check on performance in milliseconds

  Serial.println(total1_ave);                  // print sensor output 1

  if (total1_ave > 200) {
    digitalWrite(LED, LOW);
  } else if (total1_ave < 100) {
    digitalWrite(LED, HIGH);
  }

  delay(10);                             // arbitrary delay to limit data to serial port
}

