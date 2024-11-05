#include <stdio.h>
#include <stdlib.h>
#include "funcoes.h" //Algumas funções que serão usadas
#include "alocacao.h" //Para a alocação dinâmica
#include "funcoesAux.h"

void *OrdenacaoVetor(void *args) {
    struct timespec TempoInicial, TempoFinal;
    clock_gettime(CLOCK_MONOTONIC, &TempoInicial);

    //Copiando dados para variáveis locais
    parametros *param = (parametros *)args;

    /*Mutex adicionado pois a informação "QualThreadEstou" é uma região 
    crítica que deve ser acessada somente por 1 thread the cada vez*/ 
    pthread_mutex_lock(&(param->mutex));
    int QualThreadEstou = param->QualThreadEstou;
    (param->QualThreadEstou)++;
    pthread_mutex_unlock(&(param->mutex));

    int *Numeros = param->VetorNumeros;
    int NumThreads = param->NumThreads;
    int inicio = param->InicioFim[2*QualThreadEstou];
    int fim = param->InicioFim[(2*QualThreadEstou)+1];

    OrdenarParte(Numeros, inicio, fim);

    sem_post(&(param->semaforo[QualThreadEstou]));

    /*Apenas a ultima thread vai esperar todas acabarem, pois ela ordenará o todo, 
    já que, até então, somente as partes isoladas foram ordenadas*/
    if (QualThreadEstou == NumThreads-1) {
        for(int i=0; i<NumThreads-1; i++) {
            sem_wait(&(param->semaforo[i]));
        }
        
        /*Atribuimos a k o valor de número de Threads. Assim, conforme
        alteramos seu valor dentro dos if's, ele é comparado com NumThreads para 
        se identificar qual parte do MergeSort estamos*/
        int k = NumThreads;
        if (k==8) {     
            for (int i=0; i<k; i=i+2) {
                MergeSort(Numeros, param->InicioFim, i, k, NumThreads);
            }
            k=k/2;    
        }
        if (k==4) {
            for (int i=0; i<=k/2; i=i+2) {
                MergeSort(Numeros, param->InicioFim, i, k, NumThreads);
            }
            k=k/2;
        } 
        if (k==2) {
            for (int i=0; i<=k/2; i=i+2) {  
                MergeSort(Numeros, param->InicioFim, i, k, NumThreads);
                }
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &TempoFinal);

    param->TempoFinal[QualThreadEstou] = TempoFinal;
    param->TempoInicial[QualThreadEstou] = TempoInicial;

    pthread_exit(NULL);
} 

int main(int argc, char *argv[]) {
    char **ArquivosEntrada = InicializarEntrada(argc, argv);
    char *ArquivoSaida = InicializarSaida(argc, argv);
    parametros *p = InicializarParametros(argc, argv);
    pthread_t *Threads = AlocacaoDinamicaID(p->NumThreads);
    struct timespec inicio, fim;

    /*Lendo os arquivos e registrando num vetor, e
    guardando também o tamanho desse vetor*/
    int *ContFinal = AlocacaoDinamicaInt(1);
    p->VetorNumeros = AbrirEContar(ArquivosEntrada, argc-4, ContFinal);

    //Iniciando semáforos e mutex
    pthread_mutex_init(&(p->mutex), NULL);
    sem_init(&(p->semaforo[(p->NumThreads)-1]), 0, 1);
    for (int i=0; i<p->NumThreads; i++) {
        sem_init(&(p->semaforo[i]), 0, 0);
    }

    /*Vamos obter o valor do resto, e criar a variável "RestanteDoResto"
    para conseguirmos lidar com qualquer tamanho de vetor, divisível ou não*/
    int *RestanteDoResto = AlocacaoDinamicaInt(1);
    int resto = CalcularResto(RestanteDoResto, *ContFinal, p);

    for (int i=0; i<p->NumThreads; i++) {
        DefinirInicioFim(i, p, resto, RestanteDoResto, *ContFinal);

        //criando a Thread
        if (i==0)
            clock_gettime(CLOCK_MONOTONIC, &inicio);

        if (pthread_create(&Threads[i], NULL, OrdenacaoVetor, p)) {
            printf("Erro ao criar a thread %d\n", i+1);
            exit(EXIT_FAILURE);
        }
    }

    //Esperando Threads
    for (int i=0; i<p->NumThreads; i++) {
        pthread_join(Threads[i], NULL);
    }  
    clock_gettime(CLOCK_MONOTONIC, &fim);

    //Obtendo tempos  de cada thread e tempo total
    ExibirTempo(inicio, fim, p);

    //Registrando saída
    RegistrarArqSaida(ArquivoSaida, p->VetorNumeros, *ContFinal);

    //Liberando tudo que usamos e não precisamos mais
    LiberarMemoria(p, ArquivosEntrada, ArquivoSaida, argc);
    free(Threads);
    free(ContFinal);

    return 0;
}