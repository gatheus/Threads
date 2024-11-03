#ifndef FUNCOES_H
#define FUNCOES_H

//Declaração de funções
int *AbrirEContar(char **ArquivosEntrada, int NumArquivos, int *ContFinal);
void OrdenarParte(int *vetor, int inicio, int fim);
void MergeSort(int *VetorPrincipal, int *InicioFim, int QualParteEstou, int Divisoes, int NumeroThreads);
void RegistrarArqSaida(char *saida, int *vetor, int TamanhoVetor);

#endif