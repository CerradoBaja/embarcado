#include <Wire.h>
#include <RTClib.h>
#include <U8glib.h>

#define ANO_INICIAL 10
#define MES_INICIAL 10
#define DIA_INICIAL 10
#define HORA_INICIAL
#define MINUTO_INICIAL
#define SEGUNDO_INICAL

void setup () 
{
////////// INICIANDO O RTC ////////////////////
  Serial.begin(9600);
  Wire.begin();  
  rtc.begin();

  if (! rtc.isrunning()) 
  {
    Serial.println("RTC is NOT running!"); // Ve se o rtc inicializou corretamente
    rtc.adjust(DateTime(ANO_INICIAL, MES_INICIAL, DIA_INICIAL, HORA_INICIAL, MINUTO_INICIAL, SEGUNDO_INICAL)); // configura a data e hora inicial
    //Para configurar a hora seguir o exemplo
    // January 21, 2014 at 3am you would call:
    //rtc.adjust(DateTime(2016, 4, 01, 15, 36, 0));
  }
}

void loop () 
{
  if (atualMillis - anteriorMillis > 1 )
  {
    anteriorMillis = anteriorMillis + 1;
    DateTime now = rtc.now();
    dia = now.day();
    mes = now.month();
    ano = now.year();
    hora = now.hour();
    minuto = now.minute();
    segundo = now.second();
  }
  if(dia<10)
    Serial.print("0");
  Serial.print(dia);
  Serial.print('/'); 
  if(mes<10)
    Serial.print("0");
  Serial.print(mes);
  Serial.print('/');
  Serial.print(ano);
  Serial.print(' ');  
  if(hora<10)
    Serial.print("0");
  Serial.print(hora);
  Serial.print(':');
  if(minuto<10)
    Serial.print("0");
  Serial.print(minuto);
  Serial.print(':');
  if(segundo<10)
    Serial.print("0");
  Serial.print(segundo);
  Serial.println();
}

void calculoRTC(){

}
