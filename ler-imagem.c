#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int lerImagem(char nome[], char tipo[], int *M, int *largura, int *altura, int *valorMax);
    OLA
    
    char tipo[3];
    int *M, largura, altura, valorMax;
    char nome[] = "superficie_aleatoria.pgm";

    int a = lerImagem(nome, tipo, M, &largura, &altura, &valorMax);
    if (a == 1) printf("Erro ao ler o arquivo");
    
    printf("Largura: %d\n", largura);
    printf("Altura: %d\n", altura);
    printf("Valor maximo: %d\n", valorMax);
    printf("M[1][1] = %d\n", M[512]);

    return 0;
}

// Retorna 0 se dá tudo certo. Retorna outro número caso contrário.
int lerImagem(char nome[], char tipo[], int *M, int *largura, int *altura, int *valorMax) {
    
    // Abrir o arquivo
    FILE *arquivo = fopen(nome, "r");
    if (arquivo == NULL) {
        return 1;
    }

    // Ler o tipo da imagem
    fscanf(arquivo, "%s", tipo);

    // Ler largura, altura e valor máximo
    fscanf(arquivo, "%d %d", largura, altura);
    fscanf(arquivo, "%d", valorMax);

    // Cria uma matriz com as dimensões da imagem
    M = (int *)malloc((*largura) * (*altura) * sizeof(int));
    if (M == NULL) {
            printf("Erro de memória\n");
            return 1;
        }

    // Ler os pixels
    for (int i = 0; i < *altura; i++) {
        for (int j = 0; j < *largura; j++) fscanf(arquivo, "%d", &M[i*(*largura) + j]);
    }
    fclose(arquivo);
    return 0;
}
