// Inclusão de bibliotecas - INICIO 
#include <U8glib.h>
#include <MediaMovel.h>
#include <VSL_Display.h>
#include <vslThread.h>
#include "x/EEPROM.h"
#include "vslEeprom.h"
// Inclusão de bibliotecas - FIM


// Definição das constantes - INICIO
#define PIN_INTERRUPCAO_VELOCIDADE 0
#define PIN_INTERRUPCAO_RPM 4
#define PIN_TENSAO A3
#define PIN_TEMP_CVT A1
#define PIN_VELOCIDADE 2
#define PIN_RPM 19

#define TIME_THREAD_CALCULO_VELOCIDADE 500
#define TIME_THREAD_CALCULO_TEMP_TENSAO 5000
#define TIME_THREAD_CALCULO_RPM 100
#define TIME_THREAD_DISPLAY 500

#define ACTIVE_THREAD_CALCULO_VELOCIDADE true
#define ACTIVE_THREAD_CALCULO_TEMP_TENSAO false
#define ACTIVE_THREAD_CALCULO_RPM false
#define ACTIVE_THREAD_DISPLAY false

//Termos que definem quantos elementos serao utilizados para calculo da Media Movel:
#define N1 5 //Velocidade.
#define N2 5 //RPM.

#define TRANSMISSAO 4
#define PERIMETRO_RODA 0.15
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
MediaMovel RPM(N2);
MediaMovel velocidade(N1);

volatile int contadorRPM = 0;
volatile int contadorVelocidade = 0;
int aux = 0;
int tempoAnt = 0;
int lastMillis = 0;

// Definição das variáveis globais - FIM


void setup(){
	Serial.begin(9600);
	//Configuracao dos pinos:
	pinMode(PIN_TEMP_CVT, INPUT);
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
	draw(contador_filtrado, 23, 12, 300, 40, 50);
}
// Rotina para imprimir informacoes no display - FIM


//Funcao responsavel por ativar as interrupcoes - INICIO:
void ligarInterrupcoes(){
	attachInterrupt(PIN_INTERRUPCAO_VELOCIDADE, interrupcao_velocidade,  FALLING);
	attachInterrupt(PIN_INTERRUPCAO_RPM, interrupcao_rpm_motor, FALLING);
}
//Funcao responsavel por ativar as interrupcoes - FIM.

//Funcao responsavel por desativar as interrupcoes - INICIO:
void desligarInterrupcoes(){
	detachInterrupt(PIN_INTERRUPCAO_VELOCIDADE);
	detachInterrupt(PIN_INTERRUPCAO_RPM);
}
//Funcao responsavel por desativar as interrupcoes - FIM.

//Funcao responsavel pelo calculo da velocidade - INICIO:
void calculoVelocidade(){
	desligarInterrupcoes(); //Desativanto interrupcoes para que o calculo da velocidade nao seja interrompido.

	//Declaracao das variaveis usadas localmente:
	int distanciaPercorrida = contadorVelocidade * DISTANCIA_POR_INTERRUPCAO;

	//Processo para calculo da velocidade:
	velocidade.setarElementos((distanciaPercorrida/(millis() - lastMillis)/1000))*3.6);
	distancia.write(distancia.read() + distanciaPercorrida);
	contadorVelocidade = 0;
	lastMillis = millis();

	ligarInterrupcoes(); //Reativando as interrupcoes.
}
//Funcao responsavel pelo calculo da velocidade - FIM.

//Funcao responsavel pelo calculo da temperatura e da tensao da bateria - INICIO:
void calculoTempTensao()
{
	int tensao = 0;
	int temperatura1 = 0;

	//Calculo da tensao e da temperatura:
	tensao = (analogRead(A3)*0.00488*3);   // lendo o divisor de tensao (analogico 3 do arduino) e calculando a tensao.
	temperatura1 = ((analogRead(A1)*0.488)/2);  // lendo lm35 (analogico 1 do arduino) e calculando a tensao.
}
//Funcao responsavel pelo calculo da temperatura e da tensao da bateria - FIM.

//Funcao responsavel pelo calculo do RPM - INICIO:
/*void calculoRPM(){
	desligarInterrupcoes(); //Desativanto interrupcoes para que o calculo do RPM nao seja interrompido.

	RPM.setarElementos(contador_rpm*60);
	contador_rpm = 0; //Zerando o contador da interrupcao.

	ligarInterrupcoes(); //Reativando as interrupcoes.
}*/

void RPMaux(){
	if(millis() - tempoAnt > 105)
		aux = 1;
	else
		aux = 0;
	tempoAnt = millis();
}

void calculoRPM(){
	desligarInterrupcoes(); //Desativanto interrupcoes para que o calculo do RPM nao seja interrompido.
	//RPMaux();

	switch(aux){
		case 1:
			break;
		case 0:
			RPM.setarElementos(contadorRPM);
			contador_filtrado = RPM.calcularMedia();
	}
	contadorRPM = 0; //Zerando o contador da interrupcao.

	ligarInterrupcoes(); //Reativando as interrupcoes.
}

//Funcao de interrupcao da velocidade - INICIO:
void interrupcao_velocidade()
{
	Serial.println("Entrando na interrupcao");
  contadorVelocidade++;
}
//Funcao de interrupcao da velocidade - FIM.

//Funcao da interrupcao do RPM - INICIO:
void interrupcao_rpm_motor()
{ 
  contadorRPM++;
}
//Funcao da interrupcao do RPM - FIM.