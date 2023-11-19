#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "lab3.h"

extern int** sudoku_board;
int* worker_validation;

int** read_board_from_file(char* filename){
    FILE *fp = NULL;
 if (fp == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    board = (int**)malloc(sizeof(int*) * ROW_SIZE);
    if (board == NULL) {
        perror("Error allocating memory for rows");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < ROW_SIZE; i++) {
        board[i] = (int*)malloc(sizeof(int) * COL_SIZE);
        if (board[i] == NULL) {
            perror("Error allocating memory for columns");
            exit(EXIT_FAILURE);
        }

        for (int j = 0; j < COL_SIZE; j++) {
            if (fscanf(fp, "%d,", &board[i][j]) != 1) {
                perror("Error reading from file");
                exit(EXIT_FAILURE);
            }
        }
    }

    fclose(fp);
    return board;
}

int is_board_valid(){
    pthread_t* tid;  /* the thread identifiers */
    pthread_attr_t attr;
    param_struct* parameter;
    
    tid = (pthread_t*)malloc(sizeof(pthread_t) * NUM_OF_THREADS);
    if (tid == NULL) {
        perror("Error allocating memory for thread identifiers");
        exit(EXIT_FAILURE);
    }

    params = (param_struct*)malloc(sizeof(param_struct) * NUM_OF_THREADS);
    if (params == NULL) {
        perror("Error allocating memory for thread parameters");
        exit(EXIT_FAILURE);
    }

    pthread_attr_init(&attr);

    for (int i = 0; i < NUM_OF_THREADS; i++) {
        params[i].id = i;
        params[i].starting_row = i * (ROW_SIZE / NUM_OF_THREADS);
        params[i].starting_col = 0;
        params[i].ending_row = (i + 1) * (ROW_SIZE / NUM_OF_THREADS);
        params[i].ending_col = COL_SIZE;

        pthread_create(&tid[i], &attr, validate, &(params[i]));
    }

    for (int i = 0; i < NUM_OF_THREADS; i++) {
        pthread_join(tid[i], NULL);
    }

    for (int i = 0; i < NUM_OF_THREADS; i++) {
        if (worker_validation[i] == 0) {
            free(tid);
            free(params);
            pthread_attr_destroy(&attr);
            return 0;
        }
    }

    free(tid);
    free(params);
    pthread_attr_destroy(&attr);

    return 1; 
}

void* validate(void* param) {
    param_struct* params = (param_struct*)param;

    worker_validation[params->id] = 1;

    return NULL;
}
