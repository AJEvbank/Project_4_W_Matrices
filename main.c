#include "main.h"

int main(int argc, char ** argv)
{
  MPI_Init(&argc,&argv);
  int world_rank;
  MPI_Comm_rank(MCW, &world_rank);
  int world_size;
  MPI_Comm_size(MCW, &world_size);



  int n, source, i, j, row, seed, max_num, connectivity, print;

  CommLineArgs(argc,argv,&seed,&max_num,&n,&source,&connectivity,&print);

  srand(seed);
  int * edge = (int *)calloc(n * n,sizeof(int));
  int * dist = (int *)calloc(n,sizeof(int));

  if (world_rank == 0) printf("n = %d, source = %d, seed = %d, max_num = %d, connectivity = %d, print = %d\n\n",n,source,seed,max_num,connectivity,print);

  makeGraph(n,edge,max_num,connectivity);

  if (print == 1 && world_rank == 0)
  {
    for (i = -1; i < n; i++)
    {
      if (i == -1) { printf("\t "); }
      else { printf("\t%d",i); }
    }
    printf("\n");
    for (i = 0; i < n; i++)
    {
      row = i * n;
      for (j = -1; j < n; j++)
      {
        if (j == -1) { printf("\t%d",i); }
        else
        {
          if (edge[row + j] == (int)INFINITY)
          {
            printf("\t--,");
          }
          else
          {
            printf("\t%d,",edge[row + j]);
          }
        }
      }
      printf("\n");
    }
  }

  if (world_rank == 0 && print) printf("\n");
  f(source,n,edge,dist,MCW);
  if (world_rank == 0 && print) printf("\n");

  if (world_rank == 0)
  {
    for (i = 0; i < n; i++)
    {
      if (dist[i] == (int)INFINITY || dist[i] < 0)
      {
        printf("%d => --,\n",i);
      }
      else
      {
        printf("%d => %d,\n",i,dist[i]);
      }
    }
  }

  free(edge);
  free(dist);
  MPI_Finalize();
  return 0;
}
