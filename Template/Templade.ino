// Inclusão de bibliotecas - INICIO 
#include <U8glib.h>
#include <MediaMovel.h>
#include <VSL_Display.h>
#include <vslThread.h>
#include <EEPROM.h>
#include <vslEeprom.h>
#include <RTClib.h>
#include <Wire.h>

// Inclusão de bibliotecas - FIM


// Definição das constantes - INICIO
#define PIN_TENSAO A3
#define PIN_TEMP_CVT A1
#define PIN_TEMP_MOTOR A2
#define PIN_VELOCIDADE 3
#define PIN_INTERRUPCAO_VELOCIDADE 1

#define RTC_ANO_INICIAL 10
#define RTC_MES_INICIAL 10
#define RTC_DIA_INICIAL 10
#define RTC_HORA_INICIAL 10
#define RTC_MINUTO_INICIAL 10
#define RTC_SEGUNDO_INICAL 10

#define TIME_THREAD_DISPLAY 500
#define TIME_THREAD_CALCULO_VELOCIDADE 500
#define TIME_THREAD_CALCULO_TEMP_TENSAO 500

#define ACTIVE_THREAD_DISPLAY true
#define ACTIVE_THREAD_CALCULO_VELOCIDADE false
#define ACTIVE_THREAD_CALCULO_TEMP_TENSAO false

//Termos que definem quantos elementos serao utilizados para calculo da Media Movel:
#define N2 5 //Velocidade.
#define N3 5 //Temperatura CVT.
#define N4 5 //Temperatura Motor.
#define N5 5 //Tensão na bateria.

#define TRANSMISSAO 4
#define PERIMETRO_RODA 1.65
#define DISTANCIA_POR_INTERRUPCAO PERIMETRO_RODA/TRANSMISSAO

// Definição das constantes - FIM


// Definição das variáveis globais - INICIO
Thread ThreadDisplay;
Thread ThreadCalculoVelocidade;
Thread ThreadCalculoTempTensao;


ThreadControl cpuDisplay;
ThreadControl cpuSensores;
Eeprom distancia;
MediaMovel velocidade(N2);
MediaMovel tensao_bateria(N5);
MediaMovel temperatura_CVT(N3);
MediaMovel temperatura_motor(N4);
RTC_DS1307 rtc;

volatile int contadorVelocidade = 0;
unsigned long lastMillisVelocidade = 0;

// Definição das variáveis globais - FIM


void setup(){
	Serial.begin(9600);
	//Configuracao dos pinos:
	pinMode(PIN_TENSAO, INPUT);
	pinMode(PIN_VELOCIDADE, INPUT_PULLUP);
	drawStart();

	// Configuração das Threads
    ThreadCalculoVelocidade.setFunction(calculoVelocidade);
    ThreadCalculoVelocidade.setTime(TIME_THREAD_CALCULO_VELOCIDADE);
    ThreadCalculoVelocidade.active(ACTIVE_THREAD_CALCULO_VELOCIDADE);

    ThreadCalculoTempTensao.setFunction(calculoTempTensao);
    ThreadCalculoTempTensao.setTime(TIME_THREAD_CALCULO_TEMP_TENSAO);
    ThreadCalculoTempTensao.active(ACTIVE_THREAD_CALCULO_TEMP_TENSAO);

    ThreadDisplay.setFunction(plotar);
    ThreadDisplay.setTime(TIME_THREAD_DISPLAY);
    ThreadDisplay.active(ACTIVE_THREAD_DISPLAY);

	// Configuração das ThreadControls
    cpuDisplay.addThread(&ThreadDisplay);
    cpuSensores.addThread(&ThreadCalculoTempTensao);
    cpuSensores.addThread(&ThreadCalculoVelocidade);

    if (! rtc.isrunning()) 
  	{
	    Serial.println("RTC is NOT running!"); // Ve se o rtc inicializou corretamente
	    rtc.adjust(DateTime(RTC_ANO_INICIAL, RTC_MES_INICIAL, RTC_DIA_INICIAL, RTC_HORA_INICIAL, RTC_MINUTO_INICIAL, RTC_SEGUNDO_INICAL)); // configura a data e hora inicial
	    //Para configurar a hora seguir o exemplo
	    // January 21, 2014 at 3am you would call:
	    //rtc.adjust(DateTime(2016, 4, 01, 15, 36, 0));
  	}

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
	//Declaracao das variaveis utilizadas localmente:
	DateTime now = rtc.now();

	draw(23, 12, 300, 40, 50, now.hour(), now.minute());
}
// Rotina para imprimir informacoes no display - FIM


//Funcao responsavel por ativar as interrupcoes - INICIO:
void ligarInterrupcoes(){
	attachInterrupt(PIN_INTERRUPCAO_VELOCIDADE, interrupcao_velocidade,  FALLING);
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
	tensao_bateria.setarElementos((float((12.79/4.24)*(5/1023.0)*analogRead(PIN_TENSAO))));
	tensao = (analogRead(PIN_TENSAO)*0.00488*2.8169);   // lendo o divisor de tensao (analogico 3 do arduino) e calculando a tensao.
	temperatura_motor.setarElementos((float(analogRead(PIN_TEMP_MOTOR))*5/(1023))/0.01);
	temperatura_CVT.setarElementos((float(analogRead(PIN_TEMP_CVT))*5/(1023))/0.01);
	Serial.print(tensao);
	Serial.print(" | ");
	Serial.print(tensao_bateria.calcularMedia());
	Serial.print(" | ");
	Serial.println(analogRead(PIN_TENSAO));
}
//Funcao responsavel pelo calculo da temperatura e da tensao da bateria - FIM.

//Funcao de interrupcao da velocidade - INICIO:
void interrupcao_velocidade(){
	contadorVelocidade++;
}
//Funcao de interrupcao da velocidade - FIM.