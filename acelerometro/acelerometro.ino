#include <Wire.h>
#include <MMA8452.h>

void setup() {
    Serial.begin(57600);
    Serial.println("Testando");
    Wire.begin();
    initMMA8452();

}

void loop() {
    int acelCont[3];
    float acelG[3];

    readAccelData(acelCont);

    //Laco que grava os valores em acelG;
    for(int i = 0; i < 3; i++){
        acelG[i] = (float) acelCont[i] / ((1 << 12) / (2*GSCALE));
    }

    //Laco para imprimir na tela
    for(int i = 0; i < 3; i++){
        if(i == 0){
            Serial.print("x = ");
            Serial.print(acelG[i], 4);
            Serial.print("\t");
        }
        else
            if(i == 1){
                Serial.print("y = ");
                Serial.print(acelG[i], 4);
                Serial.print("\t");
            }
            else
                if(i == 2){
                    Serial.print("z = ");
                    Serial.print(acelG[i], 4);
                    Serial.print("\t");
                }
    }
    Serial.println();

    delay(500);

}

