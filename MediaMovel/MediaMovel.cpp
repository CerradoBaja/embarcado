#include "MediaMovel.h"

//Construtor:
MediaMovel::MediaMovel(int N){
	numeroElementos = N;
	oldElemento = NULL;
	newElemento = NULL;
	}

//Metodo responsavel pela criaçao de uma nova celula:
struct celula *MediaMovel::criarCelulas(float elemento){
	struct celula *cel;
	cel = (struct celula *)malloc(sizeof(struct celula));
	cel->valor = elemento;
	cel->prox = NULL;
	return cel;
}

//Metodo responsavel por preencher nossa lista com o numeroElementos desejado:
void MediaMovel::setarElementos(float elemento){
	struct celula *aux;
	int i = 1;

	if(oldElemento == NULL){
		oldElemento = criarCelulas(elemento);
		newElemento = oldElemento;
		while(i < numeroElementos){
			newElemento->prox = criarCelulas(elemento);
			newElemento = newElemento->prox;
			i++;
		}
	}
	else{
		newElemento->prox = criarCelulas(elemento);
		newElemento = newElemento->prox;

		//Processo de liberaçao do primeiro elemento:
		aux = oldElemento;
		oldElemento = oldElemento->prox;
		free(aux);
	}
}

//Metodo responsavel pelo calculo da media:
float MediaMovel::calcularMedia(){
	float aux = 0;
	float soma = 0;
	struct celula *i;
	//Laço para somar todos os N elementos da lista encadeada usados no filtro.
  	for(i = oldElemento; i != NULL; i = i->prox){
  		soma = soma + i->valor/numeroElementos;
	}
  	return soma;
}

