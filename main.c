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
  int * W0 = (int *)calloc(rootP * rootP,sizeof(int));
  int * W = (int *)calloc(rootP * rootP,sizeof(int));
  slice = n/rootP;
  start = slice * world_rank;
  end = start + slice;
  int * kthRow = (int *)calloc(n/rootP,sizeof(int));
  int * kthCol = (int *)calloc(n/rootP,sizeof(int));

  printf("n = %d, seed = %d, max_num = %d, connectivity = %d, part = %d, print = %d, full = %d\n\n",
                                                                    n,seed,max_num,connectivity,part,print,full);

  if (full == 1)
  {
    makeGraphTotal(slice,W0,max_num,connectivity,part);
  }
  else
  {
    makeGraph(slice,W0,max_num,connectivity,part);
  }

  makeGraph(slice,W,max_num,0,INF);

  printf("W0:\n");
  // Parallelize the print function.
  printGraph(slice,W0,print);

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
          W[(i * n) + j] = min(W0[(i * n) + j],addWithInfinity(kthCol[i], kthRow[j]));
        }
      }
    }

    for (i = start; i < end; i++)
    {
      for (j = start; j < end; j++)
      {
        W0[(i * n) + j] = W[(i * n) + j];
      }
    }
  }

  printf("\n\n");
  printf("W0:\n");
  printGraph(n,W0,print);
  printf("\n***************************************************************\n");
  printf("W:\n");
  printGraph(n,W,print);

  free(W0);
  free(W);
  free(kthCol);
  free(kthRow);
  MPI_Finalize();
  return 0;
}
