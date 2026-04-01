#include <stdio.h>
#include <stdlib.h>

int main(void) {
    void lerImagem(char nome[], char tipo[], int **M, int *largura, int *altura, int *valorMax, int *valorMin);
    void criaCopia(char nome_arquivo[], int *M, int **C, int largura, int altura, int valorMax, int valorMin);

    char tipo[3];
    int *M, *C, largura, altura, valorMax, valorMin;
    char original[] = "superficie_aleatoria.pgm";
    char copia[] = "copia_superficie_aleatoria.pgm";

    lerImagem(original, tipo, &M, &largura, &altura, &valorMax, &valorMin);
    
    printf("Largura: %d\n", largura);
    printf("Altura: %d\n", altura);
    printf("Valor maximo: %d\n", valorMax);
    printf("Valor minimo: %d\n", valorMin);

    criaCopia(copia, M, &C, largura, altura, valorMax, valorMin);

    return 0;
}

void lerImagem(char nome_arquivo[], char tipo[], int **M, int *largura, int *altura, int *valorMax, int *valorMin) {
    
    // Abre o arquivo
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL) {
        return;
    }

    // Lê o tipo da imagem
    fscanf(arquivo, "%s", tipo);

    // Ler largura, altura e valor máximo
    fscanf(arquivo, "%d %d", largura, altura);
    fscanf(arquivo, "%d", valorMax);

    // Cria uma matriz com as dimensões da imagem
    int *temp = (int *)malloc((*largura) * (*altura) * sizeof(int));
    *M = temp;
    if (*M == NULL) {
            printf("Erro de memória\n");
            return;
        }

    // Lê os pixels e achar o valor mínimo
    *valorMin = 255;
    for (int i = 0; i < *altura; i++) {
        for (int j = 0; j < *largura; j++) {
            fscanf(arquivo, "%d", &temp[i*(*largura) + j]);
            if (temp[i*(*largura) + j] < *valorMin) *valorMin = temp[i*(*largura) + j];
        }
    }
    fclose(arquivo);
}

// Cria um arquivo PGM contendo uma cópia da imagem já processada
void criaCopia(char nome_arquivo[], int *M, int **C, int largura, int altura, int valorMax, int valorMin) {

    // Cria um arquivo
    FILE *arquivo = fopen(nome_arquivo, "w");
    if (arquivo == NULL) {
        printf("Erro ao criar o arquivo");
        return;
    }

    // Escreve o cabeçalho do formato PGM
    fprintf(arquivo, "P2\n");
    fprintf(arquivo, "512 512\n");
    fprintf(arquivo, "255\n");

    // Cria uma cópia já processada da imagem
    int *temp = (int *)malloc(largura * altura * sizeof(int));
    *C = temp;
    if (M == NULL) {
            printf("Erro de memória\n");
            return;
        }

    int cota = (valorMax + valorMin) / 2;

    for (int i = 0; i < altura; i++) {
        for (int j = 0; j < largura; j++) {
            int m = M[i*altura + j];
            if (m <= cota) m = 0;
            else m = 255;
            temp[i*altura + j] = m;
            fprintf(arquivo, "%d ", m);
        }
        fprintf(arquivo, "\n");
    }
    fclose(arquivo);
}
