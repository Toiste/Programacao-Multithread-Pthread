#define _CRT_SECURE_NO_WARNINGS 1
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#pragma comment(lib,"pthreadVC2.lib")
#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>

#define NUM_THREADS 6
#define ALTtamanho 12000
#define LARGtamanho 12000
#define ALTmacro_bloco 500
#define LARGmacro_bloco 500
pthread_mutex_t mutex;
pthread_mutex_t mutex2;

int** matriz;
int posicao_macro = 0;
int qtd_macrobloco = (ALTtamanho * LARGtamanho) / (ALTmacro_bloco * LARGmacro_bloco);
int TotalPrimos = 0;


int ehPrimo(int n) {
    if (n < 2) {
        return 0;
    }
    int raizQuadrada = sqrt(n);
    for (int i = 2; i <= raizQuadrada; i++) {
        if (n % i == 0) {
            return 0;
        }
    }
    return 1;
}

void Alocar_matriz_real() {
    int i;
    if (ALTtamanho < 1 || LARGtamanho < 1) {
        printf("** Erro: Parametro invalido **\n");
        return;
    }

    matriz = malloc(ALTtamanho * sizeof(int*));
    if (matriz == NULL) {
        printf("** Erro: Memoria Insuficiente **");
        return;
    }

    for (i = 0; i < ALTtamanho; i++) {
        matriz[i] = malloc(LARGtamanho * sizeof(int));
        if (matriz[i] == NULL) {
            printf("** Erro: Memoria Insuficiente **");
            return;
        }
    }

}

void Liberar_matriz_real() {
    int i;
    if (matriz == NULL) return;
    if (ALTtamanho < 1 || LARGtamanho < 1) {
        printf("** Erro: Parametro invalido **\n");
        return;
    }
    for (i = 0; i < ALTtamanho; i++) free(matriz[i]);
    free(matriz);
    return;
}

void setar_matriz_real() {
    int x;

    for (int i = 0; i < ALTtamanho; i++) {
        for (int j = 0; j < LARGtamanho; j++) {
            x = rand() % 32000;
            matriz[i][j] = x;
        }
    }
}

void serial() {
    for (int i = 0; i < ALTtamanho; i++) {
        for (int j = 0; j < LARGtamanho; j++) {
            if (ehPrimo(matriz[i][j]) == 1) {
                TotalPrimos += 1;
            }
        }
    }

}

void* paralelo_thread(void* param) {
    int macrobloco_index;
    int linhaInicio;
    int linhaFim;
    int colunaInicio;
    int colunaFim;

    while (1) {
        int primoAux = 0;

        pthread_mutex_lock(&mutex);
        macrobloco_index = posicao_macro;
        posicao_macro += 1;
        pthread_mutex_unlock(&mutex);

        if (macrobloco_index >= qtd_macrobloco) {
            break;
        }

        linhaInicio = (macrobloco_index * LARGmacro_bloco) % LARGtamanho;
        linhaFim = linhaInicio + LARGmacro_bloco;
        colunaInicio = ((macrobloco_index * LARGmacro_bloco) / LARGtamanho) * LARGmacro_bloco;
        colunaFim = colunaInicio + ALTmacro_bloco;

        for (int i = colunaInicio; i < colunaFim; i++) {
            for (int linha = linhaInicio; linha < linhaFim; linha++) {
                primoAux += ehPrimo(matriz[i][linha]);
            }
        }

        pthread_mutex_lock(&mutex2);

        TotalPrimos += primoAux;

        pthread_mutex_unlock(&mutex2);
    }
    return NULL;
}


void paralelo() {
    pthread_t workers[NUM_THREADS];
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&mutex2, NULL);

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&workers[i], NULL, paralelo_thread, NULL);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(workers[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&mutex2);
}


int main(int argc, char* argv[]) {
    clock_t start_t1, end_t1, start_t2, end_t2;
    double total_t1, total_t2;
    srand(1);

    printf("Trabalho de Sistemas Operacionais: Introducao a Programacao Multithread com PThreads\n");
    printf("Alunos: \t\t Erick Komati, Marllon Ribeiro\n");
    printf("Numero de threads: \t %d \n", NUM_THREADS);
    printf("Tamanho Matriz:\t\t %d %d \n", ALTtamanho, LARGtamanho);
    printf("Tamanho do macrobloco:\t %d %d \n\n\n", ALTmacro_bloco, LARGmacro_bloco);

    Alocar_matriz_real();

    setar_matriz_real();

    start_t1 = clock();

    serial();

    end_t1 = clock();

    total_t1 = (double)(end_t1 - start_t1) / CLOCKS_PER_SEC;

    printf("A quantidade de forma serial eh: %d \n", TotalPrimos);
    printf("Tempo serial: %f \n\n", total_t1);

    TotalPrimos = 0;

    start_t2 = clock();

    paralelo();

    end_t2 = clock();

    total_t2 = (double)(end_t2 - start_t2) / CLOCKS_PER_SEC;

    printf("A quantidade de forma paralela eh: %d \n", TotalPrimos);
    printf("Tempo paralelo: %f \n\n", total_t2);


    printf("Tempo Speedup: %f \n", (total_t1 / total_t2));

    Liberar_matriz_real();

    return 0;
}
