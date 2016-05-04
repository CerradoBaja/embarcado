#include <MediaMovel.h>

MediaMovel media(5);
void setup(){
	Serial.begin(9600);
	
	media.setarElementos(100.00);
	Serial.println(media.calcularMedia());
	
	media.setarElementos(101.00);
	Serial.println(media.calcularMedia());
	
	media.setarElementos(102.00);
	Serial.println(media.calcularMedia());
	
	media.setarElementos(103.00);
	Serial.println(media.calcularMedia());
	
	media.setarElementos(104.00);
	Serial.println(media.calcularMedia());
	
	media.setarElementos(105.00);
	Serial.println(media.calcularMedia());
	
}

void loop(){
    
}
