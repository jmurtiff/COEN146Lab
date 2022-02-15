/*
# Name: Jordan Murtiff
# Date: 4/7/20
# Title: Lab2 Part 1– Multithreading in C
# Description: Exploit parallelism in large scale multidimensional matrix multiplication. 
*/
#include <stdio.h>      /* printf, stderr */
#include <stdlib.h>     /* atoi */
#include <time.h> 
#include <pthread.h>    /* pthread_create */
#include <time.h>

/* We are defining the number of columns and number of rows within the given matrices we are
multiplying together, along with the number of rows and columns for the resulting product matrix. */ 
#define N 6
#define M 5
#define L 5

/* Initalizing the creation of N threads (each thread is going to multiply a given row of the matrix) */
pthread_t tids[N];

/* Initializing matrix A and B (which are going to be multiplied together) along with matrix C
(which is going to be the products of matrices A and B) */
double matrixA[N][M], matrixB[M][L], matrixC[N][L] = {0.0};

/* The definition of the printMatrix function, which prints out a given input matrix given both the 
the number of columns, the number of rows, and the matrix itself */
void printMatrix(int nr, int nc, double matrix[nr][nc]);

/* This function acts as the start of the thread created in the main function. 
This function takes in a void pointer that acts as a given row in a given
matrix and goes along to multiply two matrices together. */
void *start_routine(void *arg)
{
    int j, k = 0;
    int i = *(int *)arg;
    for(j = 0; j < L; j++)
        for ( k = 0; k < M; k++)
            matrixC[i][j] += matrixA[i][k] * matrixB[k][j];
    free(arg);
	
}

/* The main function creates 2 matrices with random entires, and then multiples both matrices
together using threads to speed up the process and then prints out all three matrices (the two
original matrices and the product of both matrices being multiplied together.) */
int main() {
	srand(time(NULL));

    //Generating matrix A
	int i, j, k = 0;
	for (i = 0; i < N; i++) 
    {
		for (j = 0; j < M; j++) 
        {
			matrixA[i][j] = rand() % (5 + 1 - 0) + 0; // rand()
		}
	}

    //Generating matrix B
	for (i = 0; i < M; i++) 
    {
		for (j = 0; j < L; j++) {
			matrixB[i][j] = rand() % (5 + 1 - 0) + 0;; // rand()
		}
	}

	//Calculating matrix C if AxB=C using multiple threads
    for (i=0; i<N; i++){
        int *arg = malloc(sizeof(int));
        *arg = i;
        pthread_create(&tids[i], NULL, start_routine, arg);
    }

    //Printing out all three of the matrices 
	printf("\n\nMatrixA:\n");
	printMatrix(N, M, matrixA);
	printf("\n\nMatrixB:\n");
	printMatrix(M, L, matrixB);
	printf("\n\nMatrixC (multi thread):\n");
	printMatrix(N, L, matrixC); 
	
  return 0;
}

/* The printMatrix functions takes in the number of rows and columns of
a given matrix and the matrix itself and prints it to a nice format. */
void printMatrix(int nr, int nc, double matrix[nr][nc]) 
{
    int i =0;
	for (i = 0; i < nr; i++) {
        int j =0;
                for (j = 0; j < nc; j ++) {
                        printf("%lf  ", matrix[i][j]);
                }
                printf("\n");
        }
}