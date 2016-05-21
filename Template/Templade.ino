// Inclusão de bibliotecas - INICIO 
#include <U8glib.h>
#include <MediaMovel.h>
#include <VSL_Display.h>
#include <vslThread.h>
#include "x/EEPROM.h"
#include "vslEeprom.h"
// Inclusão de bibliotecas - FIM


// Definição das constantes - INICIO
#define PIN_INTERRUPCAO_VELOCIDADE 1
#define PIN_INTERRUPCAO_RPM 0
#define PIN_TENSAO A3
#define PIN_TEMP_MOTOR A0
#define PIN_TEMP_CVT A1
#define PIN_VELOCIDADE 3
#define PIN_RPM 2

#define TIME_THREAD_CALCULO_VELOCIDADE 500
#define TIME_THREAD_CALCULO_TEMP_TENSAO 5000
#define TIME_THREAD_CALCULO_RPM 100
#define TIME_THREAD_DISPLAY 500

#define ACTIVE_THREAD_CALCULO_VELOCIDADE true
#define ACTIVE_THREAD_CALCULO_TEMP_TENSAO true
#define ACTIVE_THREAD_CALCULO_RPM true
#define ACTIVE_THREAD_DISPLAY true

//Termos que definem quantos elementos serao utilizados para calculo da Media Movel:
#define N1 5 //RPM.
#define N2 5 //Velocidade.
#define N3 5 //Temperatura CVT.
#define N4 5 //Temperatura Motor.

#define TRANSMISSAO 4
#define PERIMETRO_RODA 1.65
#define DISTANCIA_POR_INTERRUPCAO PERIMETRO_RODA/TRANSMISSAO


// Definição das constantes - FIM


// Definição das variáveis globais - INICIO
Thread ThreadCalculoVelocidade;
Thread ThreadCalculoTempTensao;
Thread ThreadCalculoRPM;
Thread ThreadDisplay;


ThreadControl cpuDisplay;
ThreadControl cpuSensores;
Eeprom distancia;
MediaMovel RPM(N1);
MediaMovel velocidade(N2);
MediaMovel temperatura_CVT(N3);
MediaMovel temperatura_motor(N4);

volatile int contadorRPM = 0;
volatile int contadorVelocidade = 0;
int aux = 0;
int tempoAnt = 0;
int contador_filtrado = 50;
unsigned long lastMillisRPM = 0;
unsigned long lastMillisVelocidade = 0;

// Definição das variáveis globais - FIM


void setup(){
	Serial.begin(9600);
	//Configuracao dos pinos:
	pinMode(PIN_TENSAO, INPUT);
	pinMode(PIN_VELOCIDADE, INPUT_PULLUP);
	pinMode(PIN_RPM, INPUT_PULLUP);

	// Configuração das Threads
    ThreadCalculoVelocidade.setTime(TIME_THREAD_CALCULO_VELOCIDADE);
    ThreadCalculoVelocidade.setFunction(calculoVelocidade);
    ThreadCalculoVelocidade.active(ACTIVE_THREAD_CALCULO_VELOCIDADE);

    ThreadCalculoTempTensao.setTime(TIME_THREAD_CALCULO_TEMP_TENSAO);
    ThreadCalculoTempTensao.setFunction(calculoTempTensao);
    ThreadCalculoTempTensao.active(ACTIVE_THREAD_CALCULO_TEMP_TENSAO);

    ThreadCalculoRPM.setTime(TIME_THREAD_CALCULO_RPM);
    ThreadCalculoRPM.setFunction(calculoRPM);
    ThreadCalculoRPM.active(ACTIVE_THREAD_CALCULO_RPM);

    ThreadDisplay.setTime(TIME_THREAD_DISPLAY);
    ThreadDisplay.setFunction(plotar);
    ThreadDisplay.active(ACTIVE_THREAD_DISPLAY);

	// Configuração das ThreadControls
    cpuSensores.addThread(&ThreadCalculoVelocidade);
    cpuSensores.addThread(&ThreadCalculoTempTensao);
    cpuSensores.addThread(&ThreadCalculoRPM);
    cpuDisplay.addThread(&ThreadDisplay);

    

	ligarInterrupcoes();
}

void loop(){
	// Threads de atualização - INICIO
	cpuSensores.run();
	// Threads de atualização - FIM

	// Threads de execução - INICIO
	cpuDisplay.run();
	// Threads de execução - FIM
}

// Rotina para imprimir informacoes no display - INICIO
void plotar(){
	//draw(contador_filtrado, 23, 12, 300, 40, 50);
	Serial.print("Velocidade: ");
	Serial.print(velocidade.calcularMedia());
	Serial.print(" | ");
	Serial.print("Distancia: ");
	Serial.print(distancia.read());
	Serial.print(" | ");
	Serial.print("RPM: ");
	Serial.print(RPM.calcularMedia());
	Serial.print(" | ");
	Serial.print("Temperatura do motor: ");
	Serial.println(temperatura_motor.calcularMedia());
}
// Rotina para imprimir informacoes no display - FIM


//Funcao responsavel por ativar as interrupcoes - INICIO:
void ligarInterrupcoes(){
	attachInterrupt(PIN_INTERRUPCAO_VELOCIDADE, interrupcao_velocidade,  FALLING);
	attachInterrupt(PIN_INTERRUPCAO_RPM, interrupcao_rpm_motor, RISING);
}
//Funcao responsavel por ativar as interrupcoes - FIM.

//Funcao responsavel pelo calculo da velocidade - INICIO:
void calculoVelocidade(){

	//Declaracao das variaveis usadas localmente:
	float distanciaPercorrida = contadorVelocidade * DISTANCIA_POR_INTERRUPCAO;

	//Processo para calculo da velocidade:
	velocidade.setarElementos((distanciaPercorrida/((millis() - lastMillisVelocidade)/1000.0))*3.6);
	distancia.write(distancia.read() + distanciaPercorrida);
	contadorVelocidade = 0;
	lastMillisVelocidade = millis();
}
//Funcao responsavel pelo calculo da velocidade - FIM.

//Funcao responsavel pelo calculo da temperatura e da tensao da bateria - INICIO:
void calculoTempTensao(){
	float tensao = 0;

	//Calculo da tensao e da temperatura:
	tensao = (analogRead(A3)*0.00488*3);   // lendo o divisor de tensao (analogico 3 do arduino) e calculando a tensao.
	temperatura_motor.setarElementos((float(analogRead(PIN_TEMP_MOTOR))*5/(1023))/0.01);
	temperatura_CVT.setarElementos((float(analogRead(PIN_TEMP_CVT))*5/(1023))/0.01);
}
//Funcao responsavel pelo calculo da temperatura e da tensao da bateria - FIM.

void calculoRPM(){
	// 60000.0 -> conversao de milisegundos para minuto.
	// millis - lastMillisRPM -> variacao de tempo em milisegundos.
	// 2* -> quantidade de voltas do girabrequim por centelha da vela.
	// contadorRPM -> quantidade de interrupcoes obtidas (centelha da vela) na variacao de tempo.
	RPM.setarElementos(2*(contadorRPM/((millis() - lastMillisRPM)/60000.0)));
	contadorRPM = 0; //Zerando o contador da interrupcao.
	lastMillisRPM = millis();
}

//Funcao de interrupcao da velocidade - INICIO:
void interrupcao_velocidade(){
	contadorVelocidade++;
}
//Funcao de interrupcao da velocidade - FIM.

//Funcao da interrupcao do RPM - INICIO:
void interrupcao_rpm_motor(){ 
	contadorRPM++;
}
