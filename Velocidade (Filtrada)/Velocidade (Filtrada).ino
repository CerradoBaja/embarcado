#include <MediaMovel.h>

#define N 5
#define perimetro 155.0

volatile unsigned long tempo_t=0;
volatile float velocidade_atual = 0;
volatile float velocidade_filtrada = 0;



MediaMovel velocidade(N);

void setup() {
  	pinMode(3, INPUT_PULLUP); //  seta o pino 2 como resistor de PULLUP(interrupçao 0 do arduino mega)
  	attachInterrupt(1, interrupcao_velocidade,  FALLING);  // interrupção da velocidade 
  	Serial.begin(9600);

}

void loop() {

}
void interrupcao_velocidade()
{
	velocidade_atual = (perimetro/(millis()-tempo_t))*36;
	velocidade.setarElementos(velocidade_atual);
	velocidade_filtrada = velocidade.calcularMedia();
  	Serial.print(velocidade_atual);
  	Serial.print(" |  ");
  	Serial.print(velocidade_filtrada);
  	Serial.print(" |  ");
  	Serial.println(millis()-tempo_t);
  	tempo_t=millis();
}
