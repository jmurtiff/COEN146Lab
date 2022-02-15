/*
# Name: Jordan Murtiff
# Date: 4/7/20
# Title: Lab2 Part 1– Multithreading in C
# Description: Exploit parallelism in large scale multidimensional matrix multiplication. 
*/
/*COEN 146L : Lab1, part 1 SingleThreaded */
#include <stdio.h>
#include <stdlib.h>
#include <time.h> 
#include <pthread.h>
#include <time.h> 
#include <pthread.h>   
#include <sys/types.h>  /* pid_t */
#include <unistd.h>     /* fork */
#include <errno.h>   

#define N 10
#define M 10
#define L 10


pthread_t tids[N];

double matrixA[N][M], matrixB[M][L], matrixC[N][L];
void printMatrix(int nr, int nc, double matrix[nr][nc]);

void *start_routine(void *arg){
    int j, k = 0;
    int i = *(int *)arg;
    for(j = 0; j < L; j++)
        for ( k = 0; k < M; k++)
            matrixC[i][j] += matrixA[i][k] * matrixB[k][j];
    free(arg);
	
}

int main() {
	srand(time(NULL));
    //Generating matrix A
	int i, j, k = 0;
	for (i = 0; i < N; i++) {
		for (j = 0; j < M; j++) {
			matrixA[i][j] = rand() % 6; // rand()
		}
	}
    //Generating matrix B
	for (i = 0; i < M; i++) {
		for (j = 0; j < L; j++) {
			matrixB[i][j] = rand() % 6; // rand()
		}
	}
	//Calculating Matric C if AxB=C
    
    for (i=0; i<N; i++){
        int *arg = malloc(sizeof(int));
        *arg = i;
        pthread_create(&tids[i], NULL, start_routine, arg);
    }

    //Single thread matrix 
   /*	for(i = 0; i < N; i++){
      		for(j = 0; j < L; j++){
	           for ( k = 0; k < M; k++){
                    matrixD[i][j] += matrixA[i][k] * matrixB[k][j];
                }
            }
    }*/
	printf("\n\nMatrixA:\n");
	printMatrix(N, M, matrixA);
	printf("\n\nMatrixB:\n");
	printMatrix(M, L, matrixB);
	printf("\n\nMatrixC (multi thread):\n");
	printMatrix(N, L, matrixC); 
    //printf("\n\nMatrixD (single thread):\n");
	//printMatrix(N, L, matrixD); 
	
  return 0;
}

void printMatrix(int nr, int nc, double matrix[nr][nc]) {
    int i =0;
	for (i = 0; i < nr; i++) {
        int j =0;
                for (j = 0; j < nc; j ++) {
                        printf("%lf  ", matrix[i][j]);
                }
                printf("\n");
        }
}
