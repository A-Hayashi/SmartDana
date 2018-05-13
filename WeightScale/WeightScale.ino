#define LED_PIN 4
#define SW_PIN 5


void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  pinMode(SW_PIN, INPUT);
}


void loop(void) {
  if(digitalRead(SW_PIN)){
    Serial.println("aaa");
    digitalWrite(LED_PIN,HIGH);
    delay(1000);
    digitalWrite(LED_PIN,LOW);
    delay(1000);
  }else{
    digitalWrite(LED_PIN,HIGH);
    delay(200);
    digitalWrite(LED_PIN,LOW);
    delay(200);    
  }

}

