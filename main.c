#include "main.h"

int main(int argc, char ** argv)
{
  // MPI_Init(&argc,&argv);
  // int world_rank;
  // MPI_Comm_rank(MCW, &world_rank);
  // int world_size;
  // MPI_Comm_size(MCW, &world_size);



  int n, source, i, j, k, row, seed, max_num, connectivity, print;

  CommLineArgs(argc,argv,&seed,&max_num,&n,&source,&connectivity,&print);

  srand(seed);
  int * W0 = (int *)calloc(n * n,sizeof(int));
  int * W = (int *)calloc(n * n,sizeof(int));

  printf("n = %d, source = %d, seed = %d, max_num = %d, connectivity = %d, print = %d\n\n",n,source,seed,max_num,connectivity,print);

  makeGraph(n,W0,max_num,connectivity);

  printf("W0:\n");
  printGraph(n,WO,print);

  for (k = 0; k < n; k++)
  {
    for (i = 0; i < n; i++)
    {
      for (j = 0; j < n; j++)
      {
        W[(i * n) + j] = min(W0[(i * n) + j],W0[(i * n) + k] + W0[(k * n) + j]);
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

  printf("W0:\n");
  printGraph(n,WO,print);
  printf("\n***************************************************************\n");
  printf("W:\n");
  printGraph(n,W,print);

  free(W0);
  free(W);
  // MPI_Finalize();
  return 0;
}
