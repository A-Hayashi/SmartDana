// This example tests nilTimer1.

#include <NilRTOS.h>

// Use tiny unbuffered NilRTOS NilSerial library.
#include <NilSerial.h>

// Macro to redefine Serial as NilSerial to save RAM.
// Remove definition to use standard Arduino Serial.
#define Serial NilSerial
#include <NilTimer1.h>
//------------------------------------------------------------------------------
// Declare a stack with 64 bytes beyond context switch and interrupt needs.
NIL_WORKING_AREA(waThread1, 64);
NIL_WORKING_AREA(waThread2, 64);



#include "PS_PAD.h"

#define PS2_DAT        12  //PD12(MISO)
#define PS2_CMD        11  //PD11(MOSI)
#define PS2_CLK        13  //PD13(SCK)
#define PS2_SEL        10  //PD10(SS)

PS_PAD PAD(PS2_SEL);


#define RC522_SDA        6  //PD6(SS)
#define RC522_RESET      5  //PD5(RESET)



NIL_THREAD(Thread1, arg) {
  PAD.init();
  while (TRUE) {
    nilThdSleep(50);

    int deg;
    int lead_sw;
    PAD.poll();
    deg = PAD.read(PS_PAD::ANALOG_RX);
    Serial.print(deg);
    Serial.print("\t");
    deg = map(deg, -128, 127, 0, 180);
    Serial.println(deg);
  }
}


NIL_THREAD(Thread2, arg) {
  while (TRUE) {
    nilThdSleep(1000);
    Serial.println("====1s====");
  }
}

//------------------------------------------------------------------------------
/*
   Threads static table, one entry per thread.  A thread's priority is
   determined by its position in the table with highest priority first.

   These threads start with a null argument.  A thread's name is also
   null to save RAM since the name is currently not used.
*/
NIL_THREADS_TABLE_BEGIN()
NIL_THREADS_TABLE_ENTRY(NULL, Thread1, NULL, waThread1, sizeof(waThread1))
NIL_THREADS_TABLE_ENTRY(NULL, Thread2, NULL, waThread2, sizeof(waThread2))
NIL_THREADS_TABLE_END()
//------------------------------------------------------------------------------
void setup() {
  pinMode(RC522_SDA, OUTPUT);
  digitalWrite(RC522_SDA, HIGH);

  Serial.begin(9600);
  // start kernel
  nilSysBegin();
}
//------------------------------------------------------------------------------
// Loop is the idle thread.  The idle thread must not invoke any
// kernel primitive able to change its state to not runnable.
void loop() {
  // Not used
}
