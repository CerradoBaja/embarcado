//Biblioteca para calculo de media aritmetica.

#ifndef MEDIAMOVEL_H
#define MEDIAMOVEL_H

#include <Arduino.h>
class MediaMovel;
//Estrutura para  lista encadeada:
struct celula{
	float valor;
	struct celula *prox;
};

class MediaMovel{

	//Atributos que serao usados no arquivo .cpp
	private:
	 	int numeroElementos;
	 	struct celula *oldElemento;
	 	struct celula *newElemento;
	 	
	//Construtor e metodos:
	public:
		MediaMovel(int N);
		struct celula *criarCelulas(float elemento);
		void setarElementos(float elemento);
		float calcularMedia();
};

#endif