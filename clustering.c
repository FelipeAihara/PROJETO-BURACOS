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
void unificaBuracos(int inicio, int count, Centroides copia[], int r);
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

void unificaBuracos(int inicio, int count, Centroides copia[], int r) {
    
    // Às vezes o algoritmo pode encontra dois ou mais centroides para o mesmo buraco. Por isso a necessidade de unificá-los.
    Centroides copia_2[count - inicio];
    for (int i = 0; i < count - inicio; i++) {
        copia_2[i] = copia[inicio + i];
        printf("%d %d\n", copia_2[i].x, copia_2[i].y);
    }

    for (int i = inicio; i < count - inicio; i++) {
        for (int j = i+1; j < count - inicio; j++) {
            int dx = copia_2[i].x - copia_2[j].x;
            int dy = copia_2[i].y - copia_2[j].y;
            int d = sqrt(dx*dx + dy*dy);
            if (d < r) {
                int a, b, c, d;
                a = (copia_2[i].x + copia_2[j].x) % 2;
                if (a == 1) b = ((copia_2[i].x + copia_2[j].x) / 2) + 1;
                else b = (copia_2[i].x + copia_2[j].x) / 2;
                c = (copia_2[i].y + copia_2[j].y) % 2;
                if (c == 1) d = ((copia_2[i].y + copia_2[j].y) / 2) + 1;
                else d = (copia_2[i].y + copia_2[j].y) / 2;
                copia[inicio + 1].x = b;
                copia[inicio + 1].y = d;
                int l = 1;
                for (int k = 0; k < count - inicio; k++) {
                    if (k != i && k != j) {
                        copia[inicio + 1 + l] = copia_2[k];
                        l++;
                    }
                }
                inicio++;
                unificaBuracos(inicio, count, copia, r);
                return;
            }
        }
    }
    int n_buracos = count - inicio;
    printf("Numero de buracos: %d\n", n_buracos);
    for (int i = 0; i < n_buracos; i++) 
        printf("Buraco %d: [%d, %d]\n", i + 1, copia[inicio + i].x, copia[inicio + i].y);
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

    int inicio = 0;
    unificaBuracos(inicio, count, copia, r);
}
