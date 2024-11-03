#include "alocacao.h"
#include <stdio.h>
#include <stdlib.h>

int *AlocacaoDinamicaInt(int tamanho) {
    int *vetor = (int*)malloc(tamanho*sizeof(int));
    if (vetor == NULL) {
        printf("Alocação falhou.\n");
        exit(EXIT_FAILURE);
    }
    else return vetor;
}

char *AlocacaoDinamicaChar(int tamanho) {
    char *vetor = (char*)malloc(tamanho*sizeof(char));
    if (vetor == NULL) {
        printf("Alocação falhou.\n");
        exit(EXIT_FAILURE);
    }
    else return vetor;
}

pthread_t *AlocacaoDinamicaID(int tamanho) {
    pthread_t *vetor = (pthread_t*)malloc(tamanho*sizeof(pthread_t));
    if (vetor == NULL) {
        printf("Alocação falhou.\n");
        exit(EXIT_FAILURE);
    }
    else return vetor;
}

parametros *AlocacaoDinamicaStruct(int tamanho) {
     parametros *p = (parametros*)malloc(tamanho*sizeof(parametros));
    if (p == NULL) {
        printf("Alocação falhou.\n");
        exit(EXIT_FAILURE);
    }
    else return p;
}

sem_t *AlocacaoDinamicaSemaforo(int tamanho) {
    sem_t *vetor = (sem_t*)malloc(tamanho*sizeof(sem_t));
    if (vetor == NULL) {
        printf("Alocação falhou.\n");
        exit(EXIT_FAILURE);
    }
    else return vetor;
}