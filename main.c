#include "main.h"

int main(int argc, char ** argv)
{
  MPI_Init(&argc,&argv);
  int world_rank;
  MPI_Comm_rank(MCW, &world_rank);
  int world_size;
  MPI_Comm_size(MCW, &world_size);



  int n, i, j, k, seed, max_num, connectivity, print, part, full, rootP, slice, isDiagProcess;

  CommLineArgs(argc,argv,&seed,&max_num,&n,&connectivity,&part,&full,&print);

  srand(seed * world_rank);
  rootP = (int)sqrt((double)world_size);
  slice = n/rootP;
  isDiagProcess = isDiagonalProcess(world_rank,rootP);
  int * Origin = (int *)calloc(slice * slice,sizeof(int));
  int * Result = (int *)calloc(slice * slice,sizeof(int));
  int * kthRow = (int *)calloc(slice,sizeof(int));
  int * kthCol = (int *)calloc(slice,sizeof(int));

  if (world_rank == 0) printf("n = %d, seed = %d, max_num = %d, connectivity = %d, part = %d, print = %d, full = %d\n\n",
                                                                    n,seed,max_num,connectivity,part,print,full);

  if (full == 1)
  {
    makeGraphTotal(slice,Origin,max_num,connectivity,part);
  }
  else
  {
    makeGraph(slice,Origin,max_num,connectivity,part);
  }

  makeGraph(slice,Result,max_num,0,INF);

  int * checkOriginMatrix;
  int * checkResultParallel;
  int * checkResultSequential;
  if (world_rank == 0)
  {
    checkOriginMatrix = (int *)calloc(n * n,sizeof(int));
    checkResultParallel = (int *)calloc(n * n,sizeof(int));
    checkResultSequential = (int *)calloc(n * n,sizeof(int));
    makeGraph(n,checkResultSequential,max_num,0,INF);
  }
  else {
    checkOriginMatrix = NULL;
    checkResultParallel = NULL;
  }
  ParallelizeMatrix(MCW,Origin,slice,n,rootP,checkOriginMatrix);

  if (world_rank == 0 && print == 1)
  {
    printf("checkOriginMatrix:\n");
    printGraph(n,checkOriginMatrix,print);
  }


  // Parallel execution of the algorithm.
  for (k = 0; k < n; k++)
  {
    // Parallelize kthRow and kthCol here.
    getkRowAndCol(MCW,n,k,kthCol,kthRow,Origin);

    for (i = 0; i < slice; i++)
    {
      for (j = 0; j < slice; j++)
      {
        if (isDiagProcess && i != j)
        {
          Result[(i * slice) + j] = min(Origin[(i * slice) + j],addWithInfinity(kthCol[i], kthRow[j]));
        }
        else if (!isDiagProcess)
        {
          Result[(i * slice) + j] = min(Origin[(i * slice) + j],addWithInfinity(kthCol[i], kthRow[j]));
        }
      }
    }

    for (i = 0; i < slice; i++)
    {
      for (j = 0; j < slice; j++)
      {
        Origin[(i * slice) + j] = Result[(i * slice) + j];
      }
    }
  }


  ParallelizeMatrix(MCW,Result,slice,n,rootP,checkResultParallel);
  if (world_rank == 0 && print)
  {
    printf("checkResultParallel:\n");
    printGraph(n,checkResultParallel,print);
  }

  // Sequential execution of the algorithm on process 0.
  if (world_rank == 0)
  {
    for (k = 0; k < n; k++)
    {
      for (i = 0; i < n; i++)
      {
        for (j = 0; j < n; j++)
        {
          if (i != j)
          {
            checkResultSequential[(i * n) + j] = min(checkOriginMatrix[(i * n) + j],addWithInfinity(checkOriginMatrix[(i * n) + k],
                                                                                                checkOriginMatrix[(k * n) + j]));
          }
        }
      }

      for (i = 0; i < n; i++)
      {
        for (j = 0; j < n; j++)
        {
          checkOriginMatrix[(i * n) + j] = checkResultSequential[(i * n) + j];
        }
      }
    }

    // Compare the results and display the differences.
    int isCorrect = 1;
    for (i = 0; i < n; i++)
    {
      for (j = 0; j < n; j++)
      {
        if(checkResultSequential[(i * n) + j] != checkResultParallel[(i * n) + j])
        {
          if (print == 1) printf("Error found at [%d,%d]\n",i,j);
          isCorrect = 0;
        }
      }
    }

    if (print == 1)
    {
      printf("checkResultSequential:\n");
      printGraph(n,checkResultSequential,print);
      printf("\n\n");
    }

    if(isCorrect == 1)
    {
      printf("The result is correct.\n\n");
    }
    else
    {
      printf("The result is not correct.\n\n");
    }
  }

  free(Origin);
  free(Result);
  free(kthCol);
  free(kthRow);
  if (world_rank == 0)
  {
    free(checkOriginMatrix);
    free(checkResultParallel);
    free(checkResultSequential);
  }

  MPI_Finalize();
  return 0;
}
