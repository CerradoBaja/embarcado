#include <vslEeprom.h>

Eeprom x;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  x.write(111111);
  Serial.println(x.read());
}

void loop() {
  // put your main code here, to run repeatedly:
Serial.println(x.read());
delay(1000);
}
