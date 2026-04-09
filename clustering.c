/* NOTAS:
- Usar int ao invés de float pode resultar em erros. Ficar atento a essa possibilidade
- printf("Passou aqui\n");
- É ineficiente usar a sqrt na função clustering mas vou usar
- Vamos assumir que se d(a,b) < r e d(b,c) < r -> d(a,c) < r
- Esse algoritmo não funciona para caso haja buracos muitos pequenos muito próximos
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define K 30
#define raio 20

typedef struct {
    int x;
    int y;
    int indice;
} Pontos;

typedef struct {
    int x;
    int y;
} Centroides;

void lerImagem(char nome[], char tipo[], int **M, int *largura, int *altura, int *valorMax, int *valorMin);
void criaCopia(char nome_arquivo[], int *M, int **C, int largura, int altura, int valorMax, int valorMin, int *contador);
void clustering(int *C, int tamanho, Pontos pnt[], int len_pnt, Centroides ctr[], int len_ctr);
void unificaBuracos(Centroides copia[], int len_copia, int r);
void printaBuracos(int *C, int altura, Centroides ctr[], int len_ctr, int r);

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

    printaBuracos(C, altura, ctr, K, raio);

    free(M);
    free(C);
    
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

// Encontra as coordenadas do centro dos buracos por meio do algoritmo "k-means clustering"
void clustering(int *C, int tamanho, Pontos pnt[], int len_pnt, Centroides ctr[], int len_ctr) {

    // Encontra todos os pontos pretos, que pertencem a um buraco
    int count = 0;
    for (int i = 0; i < tamanho; i++) {
        if (C[i] == 0) {
            pnt[count].x = i % 512;
            pnt[count].y = i / 512;
            count++;
        }
    }

     // Inicializa os centroides com coordenadas aleatórias
    for (int i = 0; i < len_ctr; i++) {
        ctr[i].x = rand() % 512;
        ctr[i].y = rand() % 512;
    }

    int convergiu = 0;

    while (convergiu != len_ctr) {

        // Atribui os índices aos pontos
        for (int i = 0; i < len_pnt; i++) {\
            int menor_dist = 524288;
            for (int j = 0; j < len_ctr; j++) {
                int x = (pnt[i].x - ctr[j].x);
                int y = (pnt[i].y - ctr[j].y);
                int d = sqrt(x*x + y*y);
                if (d < menor_dist) {
                    menor_dist = d;
                    pnt[i].indice = j;
                }
            }
        }

        convergiu = 0;
        for (int i = 0; i < len_ctr; i++) {
            int cnt = 0;
            int soma_x = 0;
            int soma_y = 0;

            for (int j = 0; j < len_pnt; j++) {
                if (pnt[j].indice == i) {
                    soma_x += pnt[j].x;
                    soma_y += pnt[j].y;
                    cnt++;
                }
            }
            int x_media = ctr[i].x;
            int y_media = ctr[i].y;
            if (cnt > 0) {
                x_media = soma_x / cnt;
                y_media = soma_y / cnt;
            }
            if (x_media == ctr[i].x && y_media == ctr[i].y) 
                convergiu++;
            else {
                ctr[i].x = x_media;
                ctr[i].y = y_media;
            }
        }
    }
}

// Unifica centroides. Às vezes o algoritmo pode encontra dois ou mais centroides para o mesmo buraco. 
// Por isso a necessidade desse algoritmo.
void unificaBuracos(Centroides copia[], int len_copia, int r) {

    Centroides copia_2[len_copia];
    for (int i = 0; i < len_copia; i++) {
        copia_2[i] = copia[i];
    }
    int count = 0;
    int *Tabela = malloc(len_copia * len_copia * sizeof(int));
    int soma_linha[len_copia];
    int visto[len_copia];

    for (int i = 0; i < len_copia; i++) {
        soma_linha[i] = 0;
        visto[i] = 0;
    }

    for (int i = 0; i < len_copia; i++) {
        for (int j = 0; j < len_copia; j++) {
            int dx = copia[i].x - copia[j].x;
            int dy = copia[i].y - copia[j].y;
            int d = sqrt(dx*dx + dy*dy);
            if (d < r) {
                Tabela[i * len_copia + j] = 1;
                soma_linha[i] += 1;
            }
            else Tabela[i * len_copia + j] = 0;
        }
    }

    for (int i = 0; i < len_copia; i++) {
        if (visto[i] == 0) {
            int soma_x = 0;
            int soma_y = 0;
            int indices[soma_linha[i]];
            int temp = 0;
            for (int j = 0; j < len_copia; j++) {
                if (Tabela[i * len_copia + j] == 1) {
                    visto[j] = 1;
                    indices[temp] = j;
                    temp++;
                }
            }

            for (int k = 0; k < soma_linha[i]; k++) {
                soma_x += copia_2[indices[k]].x;
                soma_y += copia_2[indices[k]].y;
            }

            copia[count].x = soma_x / soma_linha[i];
            copia[count].y = soma_y / soma_linha[i];
            count++;
        }
    }

    // Printa a quantidade de buracos e suas coordenadas
    printf("Numero de buracos: %d\n", count);
    for (int i = 0; i < count; i++) {
        printf("Buraco %d: [%d, %d]\n", i+1, copia[i].x, copia[i].y);
    }
}

// Printa o número de buracos e as suas coordenadas
void printaBuracos(int *C, int altura, Centroides ctr[], int len_ctr, int r) {

    // Encontra todos os centroides cujo pixel é preto
    int count = 0;
    Centroides copia[len_ctr];
    for (int i = 0; i < len_ctr; i++) {
        if (C[ctr[i].y * altura + ctr[i].x] == 0) {
            copia[count] = ctr[i];
            count++;
        }
    }
    unificaBuracos(copia, count, r);
}
