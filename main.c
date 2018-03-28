#include "main.h"

int main(int argc, char ** argv)
{
  // MPI_Init(&argc,&argv);
  // int world_rank;
  // MPI_Comm_rank(MCW, &world_rank);
  // int world_size;
  // MPI_Comm_size(MCW, &world_size);



  int n, source, i, j, k, seed, max_num, connectivity, print, part, full;

  CommLineArgs(argc,argv,&seed,&max_num,&n,&source,&connectivity,&part,&full,&print);

  srand(seed);
  int * W0 = (int *)calloc(n * n,sizeof(int));
  int * W = (int *)calloc(n * n,sizeof(int));

  printf("n = %d, source = %d, seed = %d, max_num = %d, connectivity = %d, part = %d, print = %d, full = %d\n\n",
                                                                    n,source,seed,max_num,connectivity,part,print,full);

  if (full == 1)
  {
    makeGraphTotal(n,W0,max_num,connectivity,part);
  }
  else
  {
    makeGraph(n,W0,max_num,connectivity,part);
  }

  makeGraph(n,W,max_num,0,INF);

  printf("W0:\n");
  printGraph(n,W0,print);

  for (k = 0; k < n; k++)
  {
    for (i = 0; i < n; i++)
    {
      for (j = 0; j < n; j++)
      {
        if (i != j)
        {
          W[(i * n) + j] = min(W0[(i * n) + j],addWithInfinity(W0[(i * n) + k], W0[(k * n) + j]));
        }
      }
    }

    for (i = 0; i < n; i++)
    {
      for (j = 0; j < n; j++)
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
  // MPI_Finalize();
  return 0;
}
