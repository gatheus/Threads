#include "funcoesAux.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

parametros *InicializarParametros(int argc, char **argv) {
    parametros *p;

    p = (parametros*)malloc(sizeof(parametros));
    if (p == NULL) {
        printf("Alocação falhou.\n");
        exit(EXIT_FAILURE);
    }

    p->NumThreads = atoi(argv[1]);
    if (p->NumThreads!=1 && p->NumThreads!=2 && p->NumThreads!=4 && p->NumThreads!=8) {
        printf("Numero de threads deve ser 2, 4 ou 8.\n");
        exit(EXIT_FAILURE);
    }

    //Alocando os componentes da struct que são vetores
    p->semaforo = AlocacaoDinamicaSemaforo(p->NumThreads);
    p->InicioFim = AlocacaoDinamicaInt(2*(p->NumThreads));
    p->TempoFinal = (struct timespec*)malloc((p->NumThreads)*sizeof(struct timespec));
    p->TempoInicial = (struct timespec*)malloc((p->NumThreads)*sizeof(struct timespec));

    return p;
}

char **InicializarEntrada(int argc, char **argv) {
    char **ArquivosEntrada;

    //Alocando dinamicamente
    ArquivosEntrada = (char**)malloc((argc-4)*sizeof(char*));
    if (ArquivosEntrada == NULL) {
        printf("Alocação falhou.\n");
        exit(EXIT_FAILURE);
    }

    for (int i=0; i<argc-4; i++) {
        ArquivosEntrada[i] = AlocacaoDinamicaChar(100);
    }

    //Passando os dados de argv[] para o vetor
    for (int i=0; i<argc-4; i++) {
        strcpy(ArquivosEntrada[i], argv[i+2]);
    }

    return ArquivosEntrada;
}

char *InicializarSaida(int argc, char **argv) {
    char *ArquivoSaida = AlocacaoDinamicaChar(100);
    strcpy(ArquivoSaida, argv[argc-1]);

    return ArquivoSaida;
}

int CalcularResto(int *RestanteResto, int ContFinal, parametros *p) {
    int resto = (ContFinal)%(p->NumThreads);
    if (resto!=0) {
        (*RestanteResto)=resto;
    }
    else { //Para caso seja divisivel, essa variavel nao sera usada
        (*RestanteResto) = -1;
    }

    return resto;
}

void DefinirInicioFim(int i, parametros *p, int resto, int *RestanteResto, int ContFinal) {  
    //Obs: posições pares definem os inícios, e ímpares, o fim
    int razao = (ContFinal)/(p->NumThreads);
    p->InicioFim[2*i] = i*razao;
    p->InicioFim[(2*i)+1] = ((i+1)*razao)-1;

    //Para caso o tamanho do vetor nao seja divisivel pelo numero de threads:
    if ((*RestanteResto)==0) {
            p->InicioFim[2*i]=(p->InicioFim[(2*(i-1))+1])+1;
            p->InicioFim[2*i+1]=p->InicioFim[2*i]+razao-1;
        }
    if (*RestanteResto>0) {
        p->InicioFim[(2*i)+1] += (resto-(*RestanteResto)+1); 
        p->InicioFim[2*i] += (resto-(*RestanteResto));
        (*RestanteResto)--;
    } 
}

void LiberarMemoria(parametros *p, char** ArquivosEntrada, char* ArquivoSaida, int argc) {
    for (int i=0; i<p->NumThreads; i++) {
        sem_destroy(&(p->semaforo[i]));
    }
    pthread_mutex_destroy(&(p->mutex));

    for (int i=0; i<argc-4; i++) {
        free(ArquivosEntrada[i]);
    }
    free(ArquivosEntrada);
    free(ArquivoSaida);
    free(p->VetorNumeros);
    free(p->semaforo);
    free(p->InicioFim);
    free(p->TempoInicial);
    free(p->TempoFinal);
    free(p);
}

void ExibirTempo(struct timespec inicio, struct timespec fim, parametros *p) {
    double tempoTotal;
    tempoTotal = (fim.tv_sec - inicio.tv_sec); // segundos
    tempoTotal += (fim.tv_nsec - inicio.tv_nsec) / 1e9; // nanosegundos

    double tempoCadaThread;
    for (int i=0; i<p->NumThreads; i++) {
        tempoCadaThread = ((p->TempoFinal[i]).tv_sec - (p->TempoInicial[i]).tv_sec); // segundos
        tempoCadaThread += ((p->TempoFinal[i]).tv_nsec - (p->TempoInicial[i]).tv_nsec) / 1e9; // nanosegundos
        printf("Tempo de execução da thread %d: %f segundos.\n", i, tempoCadaThread);
    } 

    printf("Tempo total de execução: %f\n", tempoTotal);

}
