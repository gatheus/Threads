#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funcoes.h" //Algumas funções que serão usadas
#include "alocacao.h" //Para a alocação dinâmica

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
    pthread_t *Threads;
    char **ArquivosEntrada;
    char *ArquivoSaida;
    parametros *p;
    struct timespec inicio, fim;

    p = AlocacaoDinamicaStruct(1);

    //Obtendo o número de threads
    p->NumThreads = atoi(argv[1]);
    if (p->NumThreads!=1 && p->NumThreads!=2 && p->NumThreads!=4 && p->NumThreads!=8) {
        printf("Numero de threads deve ser 2, 4 ou 8.\n");
        exit(EXIT_FAILURE);
    }

    /*Alocando dinamicamente a matriz dos arquivos de entrada
    e vetores do arquivo de saída e de threads*/
    ArquivosEntrada = (char**)malloc((argc-4)*sizeof(char*));
    if (ArquivosEntrada == NULL) {
        printf("Alocação falhou.\n");
        exit(EXIT_FAILURE);
    }

    for (int i=0; i<argc-4; i++) {
        ArquivosEntrada[i] = AlocacaoDinamicaChar(100);
    }
    ArquivoSaida = AlocacaoDinamicaChar(100);
    Threads = AlocacaoDinamicaID(p->NumThreads);

    //Passando os dados de argv[] para vetores
    for (int i=0; i<argc-4; i++) {
        strcpy(ArquivosEntrada[i], argv[i+2]);
    }
    strcpy(ArquivoSaida, argv[argc-1]);

    /*Lendo os arquivos e registrando num vetor, e
    guardando também o tamanho desse vetor*/
    int *ContFinal = AlocacaoDinamicaInt(1);
    p->VetorNumeros = AbrirEContar(ArquivosEntrada, argc-4, ContFinal);

    p->semaforo = AlocacaoDinamicaSemaforo(p->NumThreads);
    p->InicioFim = AlocacaoDinamicaInt(2*(p->NumThreads));
    p->TempoFinal = (struct timespec*)malloc((p->NumThreads)*sizeof(struct timespec));
    p->TempoInicial = (struct timespec*)malloc((p->NumThreads)*sizeof(struct timespec));

    //Iniciando semáforos
    sem_init(&(p->semaforo[(p->NumThreads)-1]), 0, 1);
    for (int i=0; i<p->NumThreads; i++) {
        sem_init(&(p->semaforo[i]), 0, 0);
    }

    /*Vamos obter o valor do resto, e criar a variável "RestanteDoResto"
    para conseguirmos lidar com qualquer tamanho de vetor, divisível ou não*/
    int resto = (*ContFinal)%(p->NumThreads);
    int RestanteDoResto;
    if (resto!=0) {
        RestanteDoResto=resto;
    }
    else {
        RestanteDoResto = -1;
    }

    pthread_mutex_init(&(p->mutex), NULL);
    p->QualThreadEstou = 0;

    for (int i=0; i<p->NumThreads; i++) {
        //Definindo o inicio e o fim de cada thread
        //Obs: posições pares definem os inícios, e ímpares, o fim
        int razao = (*ContFinal)/(p->NumThreads);
        p->InicioFim[2*i] = i*razao;
        p->InicioFim[(2*i)+1] = ((i+1)*razao)-1;

        //Para caso o tamanho do vetor nao seja divisivel pelo numero de threads:
        if (RestanteDoResto==0) {
                p->InicioFim[2*i]=(p->InicioFim[(2*(i-1))+1])+1;
                p->InicioFim[2*i+1]=p->InicioFim[2*i]+razao-1;
            }
        if (RestanteDoResto>0) {
            p->InicioFim[(2*i)+1] += (resto-RestanteDoResto+1); 
            p->InicioFim[2*i] += (resto-RestanteDoResto);
            RestanteDoResto--;
        } 

        //criando a Thread
        if (i==0)
            clock_gettime(CLOCK_MONOTONIC, &inicio);

        int result = pthread_create(&Threads[i], NULL, OrdenacaoVetor, p);
        if (result!=0) {
            printf("Erro ao criar a thread %d\n", i+1);
            exit(EXIT_FAILURE);
        }
    }

    //Esperando Threads
    for (int i=0; i<p->NumThreads; i++) {
        pthread_join(Threads[i], NULL);
    }  
    clock_gettime(CLOCK_MONOTONIC, &fim);
    double tempoTotal;
    tempoTotal = (fim.tv_sec - inicio.tv_sec); // segundos
    tempoTotal += (fim.tv_nsec - inicio.tv_nsec) / 1e9; // nanosegundos

    double tempoCadaThread;
    for (int i=0; i<p->NumThreads; i++) {
        tempoCadaThread = ((p->TempoFinal[i]).tv_sec - (p->TempoInicial[i]).tv_sec); // segundos
        tempoCadaThread += ((p->TempoFinal[i]).tv_nsec - (p->TempoInicial[i]).tv_nsec) / 1e9; // nanosegundos
        printf("Tempo de execução da %d thread: %f segundos.\n", i, tempoCadaThread);
    } 

    printf("Tempo total de execução: %f\n", tempoTotal);

    //Registrando saída
    RegistrarArqSaida(ArquivoSaida, p->VetorNumeros, *ContFinal);

    //Liberando tudo que usamos e não precisamos mais
    for (int i=0; i<p->NumThreads; i++) {
        sem_destroy(&(p->semaforo[i]));
    }
    pthread_mutex_destroy(&(p->mutex));

    for (int i=0; i<argc-4; i++) {
        free(ArquivosEntrada[i]);
    }
    free(ArquivosEntrada);
    free(ArquivoSaida);
    free(Threads);
    free(p->VetorNumeros);
    free(p->semaforo);
    free(p->InicioFim);
    free(p->TempoInicial);
    free(p->TempoFinal);
    free(ContFinal);
    free(p);

    return 0;
}