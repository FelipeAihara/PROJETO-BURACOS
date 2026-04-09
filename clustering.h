#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
    int x;
    int y;
    int indice;
} Pontos;

typedef struct {
    int x;
    int y;
} Centroides;

void clustering(int *C, int tamanho, Pontos pnt[], int len_pnt, Centroides ctr[], int len_ctr);
void unificaBuracos(Centroides copia[], int len_copia, int r);
void printaBuracos(int *C, int altura, Centroides ctr[], int len_ctr, int r2);