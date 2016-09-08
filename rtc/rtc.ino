#include <U8glib.h>
#include <VSL_Display.h>
#include <vslThread.h>
#include <RTClib.h>
#include <Wire.h>

#define RTC_ANO_INICIAL 2016
#define RTC_MES_INICIAL 9
#define RTC_DIA_INICIAL 8
#define RTC_HORA_INICIAL 16
#define RTC_MINUTO_INICIAL 0
#define RTC_SEGUNDO_INICAL 5

#define TIME_THREAD_DISPLAY 500

#define ACTIVE_THREAD_DISPLAY true

Thread ThreadDisplay;

ThreadControl cpuDisplay;

RTC_DS1307 rtc;

void setup(){
	Serial.begin(57600);
	Wire.begin();
	rtc.begin();

	//drawStart();

	ThreadDisplay.setFunction(plotar);
    ThreadDisplay.setTime(TIME_THREAD_DISPLAY);
    ThreadDisplay.active(ACTIVE_THREAD_DISPLAY);

    // Configuração das ThreadControls
    cpuDisplay.addThread(&ThreadDisplay);

    /*if (rtc.isrunning()) 
  	{
	    Serial.println("RTC is NOT running!"); // Ve se o rtc inicializou corretamente
	    rtc.adjust(DateTime(RTC_ANO_INICIAL, RTC_MES_INICIAL, RTC_DIA_INICIAL, RTC_HORA_INICIAL, RTC_MINUTO_INICIAL, RTC_SEGUNDO_INICAL)); // configura a data e hora inicial
	    //Para configurar a hora seguir o exemplo
	    // January 21, 2014 at 3am you would call:
	    //rtc.adjust(DateTime(2016, 4, 01, 15, 36, 0));
  	}*/
}

void loop(){
	// Threads de execução - INICIO
	cpuDisplay.run();


	// Threads de execução - FIM
}

void plotar(){
	//Declaracao das variaveis utilizadas localmente:
	DateTime now = rtc.now();

	draw(23, 12, 300, 30, 30, now.hour(), now.minute());

	if(now.day()<10)
    Serial.print("0");
    Serial.print(now.day());
    Serial.print('/'); 
    if(now.month()<10)
    Serial.print("0");
    Serial.print(now.month());
    Serial.print('/');
    Serial.print(now.year());
    Serial.print(' ');  
    if(now.hour()<10)
    Serial.print("0");
    Serial.print(now.hour());
    Serial.print(':');
    if(now.minute()<10)
    Serial.print("0");
    Serial.print(now.minute());
    Serial.print(':');
    if(now.second()<10)
    Serial.print("0");
    Serial.print(now.second());
    Serial.println();
}