#ifndef FUNCOESAUX_H
#define FUNCOESAUX_H

#include "alocacao.h"

parametros *InicializarParametros(int argc, char **argv);
char **InicializarEntrada(int argc, char **argv);
char *InicializarSaida(int argc, char **argv);
int CalcularResto(int *RestanteResto, int ContFinal, parametros *p);
void DefinirInicioFim(int i, parametros *p, int resto, int *RestanteResto, int ContFinal);
void LiberarMemoria(parametros *p, char** ArquivosEntrada, char* ArquivoSaida, int argc);
void ExibirTempo(struct timespec inicio, struct timespec fim, parametros *p);

#endif