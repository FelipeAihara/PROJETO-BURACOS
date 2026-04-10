    /* NOTAS:
    - Usar int ao invés de float pode resultar em erros. Ficar atento a essa possibilidade
    - printf("Passou aqui\n");
    - É ineficiente usar a sqrt na função clustering mas vou usar
    - Vamos assumir que se d(a,b) < r e d(b,c) < r -> d(a,c) < r
    - Esse algoritmo não funciona para caso haja buracos muitos pequenos muito próximos
    - O valor máximo está errado (ARRUMEI)
    */

    #include <stdio.h>
    #include <stdlib.h>
    #include <math.h>

    #define K 30
    #define raio_quadrado 400

    typedef struct {
        int *dados;
        int altura, largura;
        int intensidade_max;
        int max, min;
    } Imagem;

    typedef struct {
        int x;
        int y;
        int indice;
    } Pontos;

    typedef struct {
        int x;
        int y;
    } Centroides;

    void lerImagem(char nome_arquivo[], char tipo[], Imagem *img);
    void criaCopia(char nome_arquivo[], int **C, Imagem *img, int *contador);
    void clustering(int *C, Imagem *img, Pontos pnt[], int len_pnt, Centroides ctr[], int len_ctr);
    void unificaBuracos(Centroides copia[], int len_copia, int r);
    void printaBuracos(int *C, Imagem *img, Centroides ctr[], int len_ctr, int r);

    int main(void) {
        char tipo[3];
        Imagem img;
        int *C;
        int contador = 0;
        char original[] = "superficie_aleatoria.pgm";
        char copia[] = "copia_superficie_aleatoria.pgm";

        lerImagem(original, tipo, &img);

        criaCopia(copia, &C, &img, &contador);

        int tamanho = img.largura * img.altura;
        Pontos pnt[contador];
        Centroides ctr[K];
        clustering(C, &img, pnt, contador, ctr, K);

        printaBuracos(C, &img, ctr, K, raio_quadrado);

        free(img.dados);
        free(C);
        
        return 0;
    }

    void lerImagem(char nome_arquivo[], char tipo[], Imagem *img) {
        
        // Abre o arquivo
        FILE *arquivo = fopen(nome_arquivo, "r");
        if (arquivo == NULL) {
            return;
        }

        // Lê o tipo da imagem
        fscanf(arquivo, "%s", tipo);

        // Ler largura, altura e valor máximo
        fscanf(arquivo, "%d %d", &(img->largura), &(img->altura));
        fscanf(arquivo, "%d", &(img->intensidade_max));

        // Cria uma matriz com as dimensões da imagem
        img->dados = malloc(img->largura * img->altura * sizeof(int));
        if (img->dados == NULL) {
                printf("Erro de memória\n");
                return;
            }

        // Lê os pixels e achar o valor mínimo
        img->max = 0;
        img->min = 255;
        for (int i = 0; i < img->altura; i++) {
            for (int j = 0; j < img->largura; j++) {
                fscanf(arquivo, "%d", &(img->dados[i*(img->largura) + j]));
                if (img->dados[i*(img->largura) + j] < img->min) 
                    img->min = img->dados[i*(img->largura) + j];
                if (img->dados[i*(img->largura) + j] > img->max)
                    img->max = img->dados[i*(img->largura) + j];
            }
        }
        fclose(arquivo);
    }

    // Cria um arquivo PGM contendo uma cópia da imagem já processada
    void criaCopia(char nome_arquivo[], int **C, Imagem *img, int *contador) {

        // Cria um arquivo
        FILE *arquivo = fopen(nome_arquivo, "w");
        if (arquivo == NULL) {
            printf("Erro ao criar o arquivo");
            return;
        }

        // Escreve o cabeçalho do formato PGM
        fprintf(arquivo, "P2\n");
        fprintf(arquivo, "%d %d\n", img->largura, img->altura);
        fprintf(arquivo, "255\n");
        for (int i = 0; i < img->largura; i++) fprintf(arquivo, "%3d ", i);
        fprintf(arquivo, "\n");

        // Cria uma cópia já processada da imagem
        int *temp = malloc(img->largura * img->altura * sizeof(int));
        if (temp == NULL) {
            printf("Erro de memória\n");
            return;
            }
        *C = temp;
        *contador = 0;

        int cota = (img->max + img->min) / 2;

        for (int i = 0; i < img->altura; i++) {
            for (int j = 0; j < img->largura; j++) {
                int m = (img->dados)[i*img->largura + j];
                if (m <= cota) {
                    m = 0;
                    *contador = *contador + 1;
                }
                else m = 255;
                temp[i * img->largura + j] = m;
                fprintf(arquivo, "%3d ", m);
            }
            fprintf(arquivo, "\n");
        }
        fclose(arquivo);
    }

    // Encontra as coordenadas do centro dos buracos por meio do algoritmo "k-means clustering"
    void clustering(int *C, Imagem *img, Pontos pnt[], int len_pnt, Centroides ctr[], int len_ctr) {

        // Encontra todos os pontos pretos, que pertencem a um buraco
        int count = 0;
        int tamanho = img->largura * img->altura;

        for (int i = 0; i < tamanho; i++) {
            if (C[i] == 0) {
                pnt[count].x = i % img->largura;
                pnt[count].y = i / img->largura;
                count++;
            }
        }

        // Inicializa os centroides com coordenadas aleatórias
        for (int i = 0; i < len_ctr; i++) {
            ctr[i].x = rand() % img->largura;
            ctr[i].y = rand() % img->largura;
        }

        int convergiu = 0;

        while (convergiu != len_ctr) {

            // Atribui os índices aos pontos
            for (int i = 0; i < len_pnt; i++) {\
                int menor_dist = 1000000;
                for (int j = 0; j < len_ctr; j++) {
                    int x = (pnt[i].x - ctr[j].x);
                    int y = (pnt[i].y - ctr[j].y);
                    int d = x*x + y*y;
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
                int d = dx*dx + dy*dy;
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

                if (soma_linha[i] == 0) break;

                copia[count].x = soma_x / soma_linha[i];
                copia[count].y = soma_y / soma_linha[i];
                count++;
            }
        }

        // Printa a quantidade de buracos e suas coordenadas
        printf("Numero de buracos: %d\n", count);
        for (int i = 0; i < count; i++) {
            printf("Buraco %d: [%d, %d]\n", i+1, copia[i].x, copia[i].y + 1);
        }
    }

    // Printa o número de buracos e as suas coordenadas
    void printaBuracos(int *C, Imagem *img, Centroides ctr[], int len_ctr, int r) {

    // Encontra todos os centroides cujo pixel é preto
    int count = 0;
    Centroides copia[len_ctr];
    for (int i = 0; i < len_ctr; i++) {
        if (C[ctr[i].y * (*img).altura + ctr[i].x] == 0) {
            copia[count] = ctr[i];
            count++;
        }
    }
    unificaBuracos(copia, count, r);
}
