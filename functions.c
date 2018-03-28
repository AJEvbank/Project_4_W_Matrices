#ifndef FUNC

#define FUNC

#include "main.h"

void printGraph(int n, int * graph, int print)
{
  int i,j,row;
  if (print == 1)
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
          if (graph[row + j] == (int)INFINITY)
          {
            printf("\t--,");
          }
          else
          {
            printf("\t%d,",graph[row + j]);
          }
        }
      }
      printf("\n");
    }
  }
  return;
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
  if (connectivity > 0)
  {
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
  else
  {
    for (i = 0; i < n; i++)
    {
      row = i * n;
      for (j = 0; j < n; j++)
      {
        edge[row + j] = (int)INFINITY;
      }
    }
  }
  return;
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
