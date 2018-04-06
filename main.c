#include "main.h"

int main(int argc, char ** argv)
{
  MPI_Init(&argc,&argv);
  int world_rank;
  MPI_Comm_rank(MCW, &world_rank);
  int world_size;
  MPI_Comm_size(MCW, &world_size);



  int n, i, j, k, seed, max_num, connectivity, print, part, full, rootP, slice, start, end;

  CommLineArgs(argc,argv,&seed,&max_num,&n,&connectivity,&part,&full,&print);

  srand(seed);
  rootP = (int)sqrt((double)world_size);
  int * Origin = (int *)calloc(rootP * rootP,sizeof(int));
  int * Result = (int *)calloc(rootP * rootP,sizeof(int));
  slice = n/rootP;
  start = slice * world_rank;
  end = start + slice;
  int * kthRow = (int *)calloc(n/rootP,sizeof(int));
  int * kthCol = (int *)calloc(n/rootP,sizeof(int));

  printf("n = %d, seed = %d, max_num = %d, connectivity = %d, part = %d, print = %d, full = %d\n\n",
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
  int * checkResultMatrix;
  if (world_rank == 0)
  {
    checkOriginMatrix = (int *)calloc(n * n,sizeof(int));
    checkResultMatrix = (int *)calloc(n * n,sizeof(int));
  }
  else {
    checkOriginMatrix = null;
    checkResultMatrix = null;
  }
  ParallelizeMatrix(MCW,Origin,n,rootP,checkOriginMatrix);

  printf("checkOriginMatrix:\n");
  // Parallelize the print function.
  printGraph(slice,checkOriginMatrix,print);

  for (k = 0; k < n; k++)
  {
    // Parallelize kthRow and kthCol here.
    getkRowAndCol(MCW,n,k,kthCol,kthRow);
    for (i = start; i < end; i++)
    {
      for (j = start; j < end; j++)
      {
        if (i != j)
        {
          Result[(i * n) + j] = min(Origin[(i * n) + j],addWithInfinity(kthCol[i], kthRow[j]));
        }
      }
    }

    for (i = start; i < end; i++)
    {
      for (j = start; j < end; j++)
      {
        Origin[(i * n) + j] = Result[(i * n) + j];
      }
    }
  }

  ParallelizeMatrix(MCW,Result,n,rootP,checkResultMatrix);

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
            checkResultMatrix[(i * n) + j] = min(checkOriginMatrix[(i * n) + j],addWithInfinity(checkOriginMatrix[(i * n) + k],
                                                                                                checkOriginMatrix[(k * n) + j]));
          }
        }
      }

      for (i = 0; i < n; i++)
      {
        for (j = 0; j < n; j++)
        {
          checkOriginMatrix[(i * n) + j] = checkResultMatrix[(i * n) + j];
        }
      }
    }

    int isCorrect = 1;
    for (i = 0; i < n; i++)
    {
      for (j = 0; j < n; j++)
      {
        if(Result[(i * n) + j] != checkResultMatrix[(i * n) + j])
        {
          printf("Error found at [%d,%d]\n",i,j);
          isCorrect = 0;
        }
      }
    }

    printf("isCorrect = %d\n",isCorrect);
  }


  free(Origin);
  free(Result);
  free(kthCol);
  free(kthRow);
  free(checkOriginMatrix);
  free(checkResultMatrix);
  MPI_Finalize();
  return 0;
}
