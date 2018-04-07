#ifndef MAIN_H

#define MAIN_H

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "getopt.h"


#define MCW MPI_COMM_WORLD
#define SEED 1
#define MAX_NUM 100
#define QUAN (int)(((double)rand())/((double)RAND_MAX) * (double)max_num) + 1
#define PRINT 0
#define N 10
#define SOURCE 0
#define CONNECTIVITY 1
#define PART 1
#define INF (int)INFINITY
#define FULL 0



// #define TIME 0
// #define CORRECT 0
// #define CHECK printf("Check on world_rank %d\n",world_rank);

// (int)(((double)rand())/((double)RAND_MAX) * 32)
// u++
// Note: max size of int = +2147483647
// Note: min size of int = -2147483648
// Because of this, rows * cols cannot exceed 2147483647, so for a square
// matrix rows = cols = 46340.

enum isNumStates {
	INITIAL,
	PLUSORMINUS,
	ZERO,
	NUMBER,
	DECIMAL,
	ERROR
};
void CommLineArgs(int argc,
                  char ** argv,
                  int *seed,
                  int *max_num,
                  int *n,
                  int *connectivity,
                  int *part,
                  int *full,
                  int *print
                 );

int isNumber(const char * str);

void printGraph(int n, int * graph, int print);

int min(int i, int j);

int isConnected(int connectivity, int part);

void makeGraph(int n, int * edge, int max_num, int connectivity, int part);

void makeGraphTotal(int n, int * edge, int max_num, int connectivity, int part);

int addWithInfinity(int A, int B);

int getMax(int world_size);

void getkRowAndCol(MPI_Comm mcw, int n, int k, int * kthCol, int * kthRow, int * myOriginMatrix);

int getProcessRow(int world_rank, int rootP);

int getProcessCol(int world_rank, int rootP);

int getRowMember(int processRow, int rootP, int index);

int getColMember(int processCol, int rootP, int index);

int getKthRowOrigin(int k, int rootP, int processRow, int slice);

int getKthColOrigin(int k, int rootP, int processCol, int slice);

void loopOperation(int offset, int level, int * receivedArray, int rootP);

void ParallelizeMatrix(MPI_Comm mcw, int * myMatrix, int slice, int n, int rootP, int * recv);

#endif
