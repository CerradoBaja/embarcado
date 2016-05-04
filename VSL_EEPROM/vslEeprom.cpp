#include "vslEeprom.h"

Eeprom::Eeprom(){

}

void Eeprom::write(float _val){
    int bufferInt[5];
    int _value = (int) _val;
    _val -= _value;
    
    bufferInt[0] = _value / 10000;
    _value = _value % 10000;

    bufferInt[1] = _value / 1000;
    _value = _value % 1000;

    bufferInt[2] = _value / 100;
    _value = _value % 100;

    bufferInt[3] = _value / 10;
    _value = _value % 10;

    bufferInt[4] = _value / 1;
    _value = _value % 1;

    bufferInt[5] = _val / 0.1;

    EEPROM.write(0, bufferInt[0]);
    EEPROM.write(1, bufferInt[1]);
    EEPROM.write(2, bufferInt[2]);
    EEPROM.write(3, bufferInt[3]);
    EEPROM.write(4, bufferInt[4]);
    EEPROM.write(5, bufferInt[5]);
}

float Eeprom::read(){
    float _value = 0;
    byte bufferInt[6];

    bufferInt[0] = EEPROM.read(0);
    bufferInt[1] = EEPROM.read(1);
    bufferInt[2] = EEPROM.read(2);
    bufferInt[3] = EEPROM.read(3);
    bufferInt[4] = EEPROM.read(4);
    bufferInt[5] = EEPROM.read(5);
	
    _value += ((int) bufferInt[0]) * 10000;
    _value += ((int) bufferInt[1]) * 1000;
    _value += ((int) bufferInt[2]) * 100;
    _value += ((int) bufferInt[3]) * 10;
    _value += ((int) bufferInt[4]) * 1;
    _value += (bufferInt[5]) * 0.1;
  
    return _value;
}
