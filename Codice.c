//Scheletro progetto API
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
//#define MAXCOMMAND 5 //lunghezza TopK + 1
//#define AGGIUNGIGRAFO "Aggi"
//#define TOPK "TopK"
#define KLIMIT 100000 //100k max valore con array
#define ALLINVAL 13

//Creazione tipo classifica (idea = vettore di k elementi)
typedef struct nodoClassifica{
	int indiceGrafo;
	unsigned int punteggioGrafo;
	struct nodoClassifica * prossimo;
}nodoClassifica;
nodoClassifica * listaClassifica;

typedef struct{
	int indiceGrafo;
	unsigned int punteggioGrafo;
}classifica;

//Dati glob
unsigned int * matriceAdiacenza;
int indiceGrafoAttuale;
int k, nNodi;

//Funzioni
int scanIniz();
void scanVeloce(unsigned int * matrixPtr);
void aggiungiGrafo(nodoClassifica *ptrTesta);
void inserisciClassifica(nodoClassifica *ptrTesta, int indiceNuovo, unsigned int punteggioNuovo);
void topK(nodoClassifica *ptrTesta);
void aggiungiGrafoALT(classifica * classificaGrafi);
void topKALT(classifica * classificaGrafi);

int main(){

	int i = 0;
	classifica * classificaGrafi;
	classificaGrafi = (classifica *)malloc(k*2*sizeof(int));
	
	//lettura d (nNodi)

	nNodi = scanIniz();
	k = scanIniz();

	//Preparazione classifica grafi

	if(k < KLIMIT){
		classificaGrafi = (classifica *)malloc(k*2*sizeof(int));
		for(i = 0; i < k; i++)
		{
			classificaGrafi[i].indiceGrafo = -1;	
			classificaGrafi[i].punteggioGrafo = UINT_MAX;
		}
	}else{
		free(classificaGrafi);
		//crea lista
		listaClassifica = (nodoClassifica *)malloc(sizeof(nodoClassifica));
		listaClassifica->indiceGrafo = -1;
		listaClassifica->punteggioGrafo = 0;
		listaClassifica->prossimo = NULL;
	}

	//Lettura resto file
	char comando, allineamento;
	int tempInt;
	indiceGrafoAttuale = 0;
	matriceAdiacenza = (unsigned int *)malloc(nNodi*nNodi*sizeof(unsigned int));
	while(feof(stdin) == 0){

		comando = getchar_unlocked();

		if(comando == 'A'){

			//Allinea alla prossima riga
			tempInt = 0;
			while(tempInt < ALLINVAL){
				//new
				allineamento = getchar_unlocked();

				//old
				//if(!scanf("%c", &allineamento)){}

				tempInt++;
			}

			//AGGIUNTA GRAFO
			if(k < KLIMIT){
				aggiungiGrafoALT(classificaGrafi);
			}else{
				aggiungiGrafo(listaClassifica);
			}
			indiceGrafoAttuale++;

		}else if(comando == 'T'){
			//STAMPA TOPK
			if(k < KLIMIT){
				topKALT(classificaGrafi);
			}else{
				topK(listaClassifica);
			}
			for(i = 0; i < 4; i++)
			{
				allineamento = getchar_unlocked();
			}
		}else{
			//ERRORE LETTURA COMANDO
			return 0;
		}
	}

	//chiusura

	allineamento += 0;
	free(matriceAdiacenza);

	return 0;
}

void aggiungiGrafo(nodoClassifica *ptrTesta){
	//Stato nodo: 0 = da ottimizzare, 1 = ottimizzato
	unsigned int totCammino = 0, distanzaMin[nNodi], /*precedente[dim],*/ tempMin, tempIndex, percAlt;
	int i, j = 0, statoNodo[nNodi];	//Lettura matrice di adiacenza
	for(i = 0; i < nNodi; i++){
		scanVeloce(matriceAdiacenza+i*nNodi);
	}

	//Dijkstra
	//Setup vettori visita e distanza. "-1" = infinito
	//Esempio: distanzaMin[nodo] = valore

	statoNodo[0] = 1;
	distanzaMin[0] = 0;
	for(i = 1; i < nNodi; i++){
		statoNodo[i] = 0;
		if(matriceAdiacenza[i] != 0){
			distanzaMin[i] = matriceAdiacenza[i];
		}else{
			distanzaMin[i] = -1;
		}
	}

	//loop principale
	i = 0;
	//Nel while i = indice nodo in considerazione
	while(i < nNodi){
		//trova minimo in distanzaMin[]
		tempMin = UINT_MAX;
		tempIndex = 0;
		for(j = 1; j < nNodi; j++){
			if(statoNodo[j] == 0 && distanzaMin[j] != -1 && distanzaMin[j] <= tempMin){
				tempMin = distanzaMin[j];
				tempIndex = j;
			}
		}
		//Rimuovi minimo dai nodi da ottimizzare
		statoNodo[tempIndex] = 1;
		//Se la condizione è vera, non ci sono nodi raggiungibili dallo 0
		if(tempIndex == 0){
			break;
		}

		//Controllo i vicini (j = 1 perchè ignoro il nodo 0)
		for(j = 1; j < nNodi; j++){
			//Salto autoanelli
			if(j == tempIndex){
				continue;
			}
			if(matriceAdiacenza[tempIndex*nNodi+j] != 0){
				percAlt = distanzaMin[tempIndex] + matriceAdiacenza[tempIndex*nNodi+j];
				if(distanzaMin[j] == -1 || percAlt < distanzaMin[j]){
					distanzaMin[j] = percAlt;
				}
			}
		}
		i++;
	}	

	//Calcolo cammino
	for(i = 0; i < nNodi; i++){
		if(distanzaMin[i] != -1)
			totCammino += distanzaMin[i];
	}

	inserisciClassifica(listaClassifica, indiceGrafoAttuale, totCammino);
	return;
}

void topK(nodoClassifica *ptrTesta){
	//new
	int i;
	nodoClassifica * ptrTemp = ptrTesta;
	//salta primo elem
	ptrTemp = ptrTemp->prossimo;

	if(ptrTemp == NULL){
		printf("\n");
		return;
	}
	
	if(ptrTemp->punteggioGrafo == -1 || ptrTemp->indiceGrafo == -1){
		printf("\n");
		return;
	}

	printf("%d", ptrTemp->indiceGrafo);
	ptrTemp = ptrTemp->prossimo;

	for(i = 0; i < k-1 && ptrTemp != NULL; i++){
		printf(" %d", ptrTemp->indiceGrafo);
		ptrTemp = ptrTemp->prossimo;
	}

	printf("\n");
	return;
}

int scanIniz(){
	int x = 0;
	char c = getchar_unlocked();
	while(c != ' ' && c != '\n'){
		x = x*10 + (c - '0');
		c = getchar_unlocked();
	}
	return x;
}

void scanVeloce(unsigned int * matrixPtr){
	int i;
	unsigned int x;
	char c = getchar_unlocked();
	for(i = 0; i < nNodi; i++){
		x = 0;
		while(c != ',' && c != '\n'){
			x = x*10 + (c - '0');
			c = getchar_unlocked();
		}
		*(matrixPtr + i) = x;
		if(c == '\n'){
			break;
		}
		c = getchar_unlocked();
	}
	return;
}

void inserisciClassifica(nodoClassifica *ptrTesta, int indiceNuovo, unsigned int punteggioNuovo){
	
	nodoClassifica * nuovoNodo = (nodoClassifica *)malloc(sizeof(nodoClassifica));
	nuovoNodo->indiceGrafo = indiceNuovo;
	nuovoNodo->punteggioGrafo = punteggioNuovo;
	nuovoNodo->prossimo = NULL;
	nodoClassifica * ptrTemp = ptrTesta;
	nodoClassifica * ptrPrec = ptrTesta;

	while(ptrTemp->prossimo != NULL){
		if(punteggioNuovo < ptrTemp->punteggioGrafo){
			//Inserisco davanti a ptrTemp
			if(ptrTemp == ptrTesta){
				//Aggiunto in testa
				nuovoNodo->prossimo = ptrTesta;
				ptrTesta = nuovoNodo;
			}else{
				//Aggiunto davanti a ptrTemp ma dopo il precedente
				ptrPrec->prossimo = nuovoNodo;
				nuovoNodo->prossimo = ptrTemp;
			}
			return;
		}
		ptrPrec = ptrTemp;
		ptrTemp = ptrTemp->prossimo;
	}

	//Controllo su ultimo della lista
	if(punteggioNuovo < ptrTemp->punteggioGrafo){
		//printf("controllo extra su ultimo\n");
		//Aggiunto davanti a ptrTemp ma dopo il precedente
		if(ptrTemp == ptrTesta){
			//Aggiunto in testa
			nuovoNodo->prossimo = ptrTesta;
			ptrTesta = nuovoNodo;
		}else{
			ptrPrec->prossimo = nuovoNodo;
			nuovoNodo->prossimo = ptrTemp;
		}
		return;
	}
	//Aggiunto in coda
	ptrTemp->prossimo = nuovoNodo;
	return;
}

void aggiungiGrafoALT(classifica * classificaGrafi){
	//Stato nodo: 0 = da ottimizzare, 1 = ottimizzato
	unsigned int totCammino = 0, distanzaMin[nNodi], /*precedente[dim],*/ tempMin, tempIndex, percAlt, percorsoTotPeggiore;
	int i, j = 0, statoNodo[nNodi], indicePeggiore;	//Lettura matrice di adiacenza
	matriceAdiacenza = (unsigned int *)malloc(nNodi*nNodi*sizeof(unsigned int));
	for(i = 0; i < nNodi; i++){
		scanVeloce(matriceAdiacenza+i*nNodi);
	}

	//Dijkstra
	//Setup vettori visita e distanza. "-1" = infinito
	//Esempio: distanzaMin[nodo] = valore

	statoNodo[0] = 1;
	distanzaMin[0] = 0;
	for(i = 1; i < nNodi; i++){
		statoNodo[i] = 0;
		if(matriceAdiacenza[i] != 0){
			distanzaMin[i] = matriceAdiacenza[i];
		}else{
			distanzaMin[i] = -1;
		}
	}

	//loop principale
	i = 0;
	//Nel while i = indice nodo in considerazione
	while(i < nNodi){
		//trova minimo in distanzaMin[]
		tempMin = UINT_MAX;
		tempIndex = 0;
		for(j = 1; j < nNodi; j++){
			if(statoNodo[j] == 0 && distanzaMin[j] != -1 && distanzaMin[j] <= tempMin){
				tempMin = distanzaMin[j];
				tempIndex = j;
			}
		}
		//Rimuovi minimo dai nodi da ottimizzare
		statoNodo[tempIndex] = 1;
		//Se la condizione è vera, non ci sono nodi raggiungibili dallo 0
		if(tempIndex == 0){
			break;
		}

		//Controllo i vicini (j = 1 perchè ignoro il nodo 0)
		for(j = 1; j < nNodi; j++){
			//Salto autoanelli
			if(j == tempIndex){
				continue;
			}
			if(matriceAdiacenza[tempIndex*nNodi+j] != 0){
				percAlt = distanzaMin[tempIndex] + matriceAdiacenza[tempIndex*nNodi+j];
				if(distanzaMin[j] == -1 || percAlt < distanzaMin[j]){
					distanzaMin[j] = percAlt;
				}
			}
		}
		i++;
	}

	//Libera spazio, la matrice non serve più
	free(matriceAdiacenza);	

	//Calcolo cammino
	for(i = 0; i < nNodi; i++){
		if(distanzaMin[i] != -1)
			totCammino += distanzaMin[i];
	}

	if(indiceGrafoAttuale < k){
		classificaGrafi[indiceGrafoAttuale].punteggioGrafo = totCammino;
		classificaGrafi[indiceGrafoAttuale].indiceGrafo = indiceGrafoAttuale;
		return;
	}

	//Trova grafico peggiore in classifica, suo indice = indicePeggiore
	indicePeggiore = 0;
	percorsoTotPeggiore = classificaGrafi[0].punteggioGrafo;
	for(i = 1; i < k; i++){
		if(classificaGrafi[i].punteggioGrafo > percorsoTotPeggiore){
			percorsoTotPeggiore = classificaGrafi[i].punteggioGrafo;
			indicePeggiore = i;
		}
	}
	if(totCammino < percorsoTotPeggiore){
		classificaGrafi[indicePeggiore].punteggioGrafo = totCammino;
		classificaGrafi[indicePeggiore].indiceGrafo = indiceGrafoAttuale;
	}

	return;
}

void topKALT(classifica * classificaGrafi){
	int i;
	if(classificaGrafi[0].indiceGrafo == -1){
		printf("\n");
		return;
	}
	printf("%d", classificaGrafi[0].indiceGrafo);
	for(i = 1; i < k; i++){
		if(classificaGrafi[i].indiceGrafo == -1){
			printf("\n");
			return;
		}
		printf(" %d", classificaGrafi[i].indiceGrafo);
	}
	printf("\n");
	return;
}