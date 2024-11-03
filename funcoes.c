#include "funcoes.h"
#include <stdio.h>
#include <stdlib.h>
#include "alocacao.h"

int *AbrirEContar(char **ArquivosEntrada, int NumArquivos, int *ContFinal) {
    FILE *arquivo;
    int aux;
    int *cont;
    int ContTotal=0;
    int **DadosArquivos;
    int *VetorFinal;
    int i=0;

    DadosArquivos = (int**)malloc(NumArquivos*sizeof(int*));
    if (DadosArquivos == NULL) {
        printf("Alocação falhou.\n");
        exit(EXIT_FAILURE);
    }

    cont = AlocacaoDinamicaInt(NumArquivos);

    //Inicialização do vetor cont
    for (i=0; i<NumArquivos; i++) {
        cont[i]=0;
    }

    //Contagem dos numeros em cada arquivo
    for (i=0; i<NumArquivos; i++) {
        arquivo = fopen(ArquivosEntrada[i], "r");
        if (arquivo == NULL) {
            printf("Erro ao abrir o arquivo.\n");
            exit(EXIT_FAILURE);
        }

        while(fscanf(arquivo, "%d", &aux)!=EOF) {
            cont[i]++;
            ContTotal++;
        }    
        DadosArquivos[i] = AlocacaoDinamicaInt(cont[i]);

        fclose(arquivo);
    }

    //Gravação dos numeros nos vetores já alocados dinamicamente
    for (i=0; i<NumArquivos; i++) {
        arquivo = fopen(ArquivosEntrada[i], "r");
        if (arquivo == NULL) {
            printf("Erro ao abrir o arquivo.\n");
            exit(EXIT_FAILURE);
        }

        int j=0;
        while(fscanf(arquivo, "%d", &aux)!=EOF) {
            DadosArquivos[i][j]=aux;
            j++;
        }    
        
        fclose(arquivo);
    }

    //Obtenção do vetor final com todos numeros
    VetorFinal = AlocacaoDinamicaInt(ContTotal);
    i=0;
    aux=0;
    for(int j=0; j<ContTotal; j++) {
        VetorFinal[j]=DadosArquivos[i][aux];
        aux++;

        if (aux==cont[i]) {
            i++;
            aux=0;
        }
    }

    *ContFinal = ContTotal;

    //Liberando a memória do que não precisamos mais
    free(cont);
    for (i=0; i<NumArquivos; i++) {
        free(DadosArquivos[i]);
    }
    free(DadosArquivos);

    return VetorFinal;
}

void OrdenarParte(int *vetor, int inicio, int fim) {
    int aux;

    //Ordenando as partes do vetor
    for(int j=inicio; j<=fim-1; j++) {
        for (int i=j+1; i<=fim; i++) {
            if (vetor[j]>vetor[i]) {
                aux=vetor[j];
                vetor[j]=vetor[i];
                vetor[i]=aux;
            }   
        }
    }
}

void MergeSort(int *VetorPrincipal, int* InicioFim, int QualParteEstou, int Divisoes, int NumeroThreads) {

    /*Aqui, estamos misturando as partes ordenadas, comparando-as
    de 2 a 2, até obtermos o vetor final totalmente ordenado*/

    int i, j;
    int k;
    int n;
    int Posicao;
    int TamanhoVet1;
    int TamanhoVet2;

    /*Identifica em qual repetição do MergeSort estamos, e atribui 
    os tamanhos de acordo com isso.*/
    if (Divisoes==NumeroThreads) {
        k=2;
        TamanhoVet1 = InicioFim[(2*QualParteEstou)+1] - (InicioFim[2*QualParteEstou]-1);
        TamanhoVet2 = InicioFim[(2*(QualParteEstou+1))+1] - (InicioFim[2*(QualParteEstou+1)]-1);
    } else
    if (Divisoes==(NumeroThreads/2)) {
        k=4;
        TamanhoVet1 = InicioFim[(4*QualParteEstou)+3] - (InicioFim[4*QualParteEstou]-1);
        TamanhoVet2 = InicioFim[(4*(QualParteEstou+1))+3] - (InicioFim[4*(QualParteEstou+1)]-1);
    } else
    if (Divisoes==(NumeroThreads/4)) {
        k=8;
        TamanhoVet1 = InicioFim[(8*QualParteEstou)+7] - (InicioFim[8*QualParteEstou]-1);
        TamanhoVet2 = InicioFim[(8*(QualParteEstou+1))+7] - (InicioFim[8*(QualParteEstou+1)]-1);
    }

    int *Vetor1 = AlocacaoDinamicaInt(TamanhoVet1);
    int *Vetor2 = AlocacaoDinamicaInt(TamanhoVet2);

    //Copiando dados do vetor principal para as partes
    Posicao = InicioFim[k*QualParteEstou];
    for (i=0; i<TamanhoVet1; i++) {
        Vetor1[i] = VetorPrincipal[Posicao];
        Posicao++;
    }
    Posicao = InicioFim[k*(QualParteEstou+1)];
    for (i=0; i<TamanhoVet2; i++) {
        Vetor2[i] = VetorPrincipal[Posicao];
        Posicao++;
    }
 
    /*Comparando as partes com o vetor principal e ordenando-o
    seguindo o argoritmo do MergeSort*/
    i=0;
    j=0;
    Posicao = InicioFim[k*QualParteEstou];
    while (i<TamanhoVet1 && j<TamanhoVet2) {
        if (Vetor1[i]<=Vetor2[j]) {
            VetorPrincipal[Posicao] = Vetor1[i];
            i++;
        }
        else {
        VetorPrincipal[Posicao] = Vetor2[j];
        j++;
        }
        Posicao++;
    }

    //Copiando os elementos restantes de Vetor1, se houver
    while (i < TamanhoVet1) {
        VetorPrincipal[Posicao] = Vetor1[i];
        i++;
        Posicao++;
    }

    //Copiando os elementos restantes de Vetor2, se houver
    while (j < TamanhoVet2) {
        VetorPrincipal[Posicao] = Vetor2[j];
        j++;
        Posicao++;
    }

    free(Vetor1);
    free(Vetor2);
}

void RegistrarArqSaida(char *saida, int *vetor, int TamanhoVetor) {
    FILE *arquivo;
    arquivo = fopen(saida, "w");
    if (arquivo == NULL) {
            printf("Erro ao abrir o arquivo.\n");
            exit(EXIT_FAILURE);
        }

    for (int i=0; i<TamanhoVetor; i++) {
        fprintf(arquivo, "%d\n", vetor[i]);
    }

    fclose(arquivo);
}
