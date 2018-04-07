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

  srand(seed * world_rank);
  rootP = (int)sqrt((double)world_size);
  slice = n/rootP;
  start = slice * world_rank;
  end = start + slice;
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
  int * checkResultMatrix;
  int * checkResultSequential;
  if (world_rank == 0)
  {
    checkOriginMatrix = (int *)calloc(n * n,sizeof(int));
    checkResultMatrix = (int *)calloc(n * n,sizeof(int));
    checkResultSequential = (int *)calloc(n * n,sizeof(int));
    makeGraph(n,checkResultSequential,max_num,0,INF);
  }
  else {
    checkOriginMatrix = NULL;
    checkResultMatrix = NULL;
  }
  ParallelizeMatrix(MCW,Origin,slice,n,rootP,checkOriginMatrix);

  if (world_rank == 0)
  {
    printf("checkOriginMatrix:\n");
    printGraph(n,checkOriginMatrix,print);
    // printf("checkResultMatrix:\n");
    // printGraph(n,checkResultMatrix,print);

  }
  // printf("Origin on %d:\n",world_rank);
  // printGraph(slice,Origin,print);


  for (k = 0; k < n; k++)
  {
    // Parallelize kthRow and kthCol here.
    getkRowAndCol(MCW,n,k,kthCol,kthRow,Origin);

    // printf("%d => kthCol: ",world_rank);
    // for (i = 0; i < slice; i++)
    // {
    //   if (kthCol[i] == INF)
    //   {
    //     printf("--,");
    //   }
    //   else
    //   {
    //     printf("%d,",kthCol[i]);
    //   }
    //
    //   // for (j = start; j < end; j++)
    //   // {
    //   //   if (i != j)
    //   //   {
    //   //     //Result[(i * n) + j] = min(Origin[(i * n) + j],addWithInfinity(kthCol[i], kthRow[j]));
    //   //   }
    //   // }
    // }
    // printf("\n");
    // printf("%d => kthRow: ",world_rank);
    // for (i = 0; i < slice; i++)
    // {
    //   if (kthRow[i] == INF)
    //   {
    //     printf("--,");
    //   }
    //   else
    //   {
    //     printf("%d,",kthRow[i]);
    //   }
    //
    //   // for (j = start; j < end; j++)
    //   // {
    //   //   //Origin[(i * n) + j] = Result[(i * n) + j];
    //   // }
    // }
    // printf("\n");

    if(k == 1) break;
  }

  // printf("Result on %d:\n",world_rank);
  // printGraph(slice,Result,print);
  // ParallelizeMatrix(MCW,Result,slice,n,rootP,checkResultMatrix);
  if (world_rank == 0)
  {
    // printf("checkResultMatrix:\n");
    // printGraph(n,checkResultMatrix,print);
  }

  // if (world_rank == 0)
  // {
  //   for (k = 0; k < n; k++)
  //   {
  //     for (i = 0; i < n; i++)
  //     {
  //       for (j = 0; j < n; j++)
  //       {
  //         if (i != j)
  //         {
  //           checkResultSequential[(i * n) + j] = min(checkOriginMatrix[(i * n) + j],addWithInfinity(checkOriginMatrix[(i * n) + k],
  //                                                                                               checkOriginMatrix[(k * n) + j]));
  //         }
  //       }
  //     }
  //
  //     for (i = 0; i < n; i++)
  //     {
  //       for (j = 0; j < n; j++)
  //       {
  //         checkOriginMatrix[(i * n) + j] = checkResultSequential[(i * n) + j];
  //       }
  //     }
  //   }
  //
  //   int isCorrect = 1;
  //   // for (i = 0; i < n; i++)
  //   // {
  //   //   for (j = 0; j < n; j++)
  //   //   {
  //   //     if(checkResultSequential[(i * n) + j] != checkResultMatrix[(i * n) + j])
  //   //     {
  //   //       printf("Error found at [%d,%d]\n",i,j);
  //   //       isCorrect = 0;
  //   //     }
  //   //   }
  //   // }
  //
  //   printf("isCorrect = %d\n",isCorrect);
  //   printf("checkResultSequential:\n");
  //   printGraph(n,checkResultSequential,print);
  // }

  free(Origin);
  free(Result);
  free(kthCol);
  free(kthRow);
  if (world_rank == 0)
  {
    free(checkOriginMatrix);
    free(checkResultMatrix);
    free(checkResultSequential);
  }

  MPI_Finalize();
  return 0;
}
