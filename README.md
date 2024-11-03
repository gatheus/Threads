# Projeto Multithreading
Nesse repositório, se encontram todos arquivos usados para a construção do programa requisitado.
É importante lembrar que este README é somente uma introdução. Caso queira ver o relatório completo de todos resultados obtidos, clique [aqui](Relatório do Projeto.pdf).

## Breve introdução
Para uma melhor modularização, dividi o código em três partes: 
1. [code.c](code.c) - Contém a função *int main()* e 
a função *OrdenacaoVetor*, que será utilizada por cada uma das Threads.
2. [funcoes.c](funcoes.c) - Contém as funções principais do programa, como funções de
ordenação e funções para ler e criar arquivos. Esse arquivo possui seu *header*: [funcoes.h](funcoes.h)
3. [alocacao.c](alocacao.c) - Contém funções de alocação dinâmica, já que em diversas partes do código,
foi preciso alocar dinamicamente desde inteiros até structs inteiras. Esse arquivo possui seu *header*: [alocacao.h](alocacao.h)

E para deixar a compilação mais fácil, criei tambem um arquivo [Makefile](Makefile)

**Para acessar cada um dos arquivos, basta clicar sobre os textos em azul.**
