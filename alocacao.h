#ifndef ALOCACAO_H
#define ALOCACAO_H

#include <pthread.h>
#include <semaphore.h>
#include <time.h> //Para a contagem dos tempos de execução

typedef struct {
    int *VetorNumeros;
    int NumThreads;
    int QualThreadEstou;
    int *InicioFim;
    struct timespec *TempoInicial;
    struct timespec *TempoFinal;
    sem_t * semaforo;
    pthread_mutex_t mutex;
} parametros;

// Declaração de funções
int *AlocacaoDinamicaInt(int tamanho);
char *AlocacaoDinamicaChar(int tamanho);
pthread_t *AlocacaoDinamicaID(int tamanho);
parametros *AlocacaoDinamicaStruct(int tamanho);
sem_t *AlocacaoDinamicaSemaforo(int tamanho);

#endif