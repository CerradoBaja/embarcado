#ifndef VSLEEPROM_H
#define VSLEEPROM_H

#include <Arduino.h>
#include <inttypes.h>
#include "x/EEPROM.h"

class Eeprom{
    private:
    public:
        // Constructor
        Eeprom();

        void write(float _value);

        float read();
};
#endif
