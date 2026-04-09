/* NOTAS:
- Usar int ao invés de float pode resultar em erros. Ficar atento a essa possibilidade
- printf("Passou aqui\n");
- É ineficiente usar a sqrt na função clustering mas vou usar
- Vamos assumir que se d(a,b) < r e d(b,c) < r -> d(a,c) < r
- Esse algoritmo não funciona para caso haja buracos muitos pequenos muito próximos
*/

#include <stdio.h>
#include <stdlib.h>
#include "imagem.h"
#include "clustering.h"

#define K 30
#define raio_quadrado 20

int main(void) {

    char tipo[3];
    int *M, *C, largura, altura, valorMax, valorMin;
    int contador = 0;
    char original[] = "superficie_aleatoria.pgm";
    char copia[] = "copia_superficie_aleatoria.pgm";

    lerImagem(original, tipo, &M, &largura, &altura, &valorMax, &valorMin);

    criaCopia(copia, M, &C, largura, altura, valorMax, valorMin, &contador);

    int tamanho = largura * altura;
    Pontos pnt[contador];
    Centroides ctr[K];
    clustering(C, tamanho, pnt, contador, ctr, K);

    printaBuracos(C, altura, ctr, K, raio_quadrado);

    free(M);
    free(C);
    
    return 0;
}
