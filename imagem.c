#include "imagem.h"

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
    int *temp = malloc((*largura) * (*altura) * sizeof(int));
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
void criaCopia(char nome_arquivo[], int *M, int **C, int largura, int altura, int valorMax, int valorMin, int *contador) {

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
    for (int i = 0; i < 512; i++) fprintf(arquivo, "%3d ", i);
    fprintf(arquivo, "\n");

    // Cria uma cópia já processada da imagem
    int *temp = malloc(largura * altura * sizeof(int));
    *C = temp;
    *contador = 0;
    if (M == NULL) {
            printf("Erro de memória\n");
            return;
        }

    int cota = (valorMax + valorMin) / 2;

    for (int i = 0; i < altura; i++) {
        for (int j = 0; j < largura; j++) {
            int m = M[i*altura + j];
            if (m <= cota) {
                m = 0;
                *contador = *contador + 1;
            }
            else m = 255;
            temp[i*altura + j] = m;
            fprintf(arquivo, "%3d ", m);
        }
        fprintf(arquivo, "\n");
    }
    fclose(arquivo);
}