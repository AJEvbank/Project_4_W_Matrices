#ifndef FUNC

#define FUNC

#include "main.h"

void f(
        int source,
        int n,
        int *edge,
        int *dist,
        MPI_Comm mcw
      )
{
  int world_rank;
  MPI_Comm_rank(mcw, &world_rank);
  int world_size;
  MPI_Comm_size(mcw, &world_size);

  int i, j, count, slice = n/world_size, start = world_rank * slice;
  int * found = (int *) calloc( n, sizeof(int) );
  for(i = 0; i < n; i++)
  {
    found[i] = 0;
    dist[i] = edge[(source * n) + i];
  }

  found[source] = 1;
  count = 1;
  while( count < n )
  {

    if (DEBUG_F) printf("choosing on %d\n",world_rank);

    j = choose(mcw, dist, n, found);
    found[j] = 1;
    count++;

    if (DEBUG_CHOOSE) printf("j = %d, count = %d on %d\n",j,count,world_rank);

    for(i = start; i < start + slice; i++)
    {

      if (DEBUG_F) printf("i = %d on %d\n",i,world_rank);

      if ( (found[i]) == 0 )
      {
        if (DEBUG_F) { printf("On %d => dist[%d] = min(%d,(%d + %d = %d));\n",
                              world_rank,
                              i,
                              dist[i],
                              dist[j],
                              edge[(j * n) + i],
                              addWithInfinity(dist[j],edge[(j * n) + i])); }

        dist[i] = min(dist[i], addWithInfinity(dist[j],edge[(j * n) + i]));

        if (DEBUG_F) { printf("On %d => dist[%d] = %d;\n",world_rank,i,dist[i]); }

      }
    }
    Reduce_Bcast_Distances(mcw,dist,n);
  }
  free(found);
  return;
}

int choose(MPI_Comm mcw, int *dist, int n, int *found)
{
  int world_rank;
  MPI_Comm_rank(mcw, &world_rank);
  int world_size;
  MPI_Comm_size(mcw, &world_size);

  int i, tmp, slice = n/world_size, start = world_rank * slice, localMinimum = -1, least = (int)INFINITY;
  for(i = start; i < start + slice; i++)
  {
      tmp = dist[i];
      if( (!found[i]) && (tmp < least) ) {
        if (DEBUG_CH_LOOP)
        {
          printf("On %d => found[%d] = %d && %d < %d\n",world_rank,i,found[i],tmp,least);
        }
        least = tmp;
        localMinimum = i;
        if (DEBUG_CH_LOOP)
        {
          printf("On %d => least = %d, localMinimum = %d\n",world_rank,least,localMinimum);
        }
      }
  }
  return Reduce_Bcast_J(mcw,localMinimum,dist,n);
}

int min(int i, int j)
{
  if (i < j)
  {
    return i;
  }
  return j;
}

int isConnected(int connectivity)
{
  return ((rand() % connectivity) == 0);
}

void makeGraph(int n, int * edge, int max_num, int connectivity)
{
  int i,j,row;
  for (i = 0; i < n; i++)
  {
    row = i * n;
    for (j = 0; j < n; j++)
    {
      if (i != j && isConnected(connectivity))
      {
        edge[row + j] = QUAN;
      }
      else
      {
        edge[row + j] = (int)INFINITY;
      }
    }
  }
}

int addWithInfinity(int A, int B)
{
  if (A == (int)INFINITY)
  {
    return A;
  }
  else if (B == (int)INFINITY)
  {
    return B;
  }
  else
  {
    return (A + B);
  }
}

int getMax(int world_size)
{
  int max = 2;
  while (max < world_size)
  {
    max *= 2;
  }
  return max;
}

#endif
