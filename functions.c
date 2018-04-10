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

void printArray(int * array, int length)
{
  int i;
  for(i = 0; i < length; i++)
  {
    printf("%d,",array[i]);
  }
  return;
}

void printValue(int value)
{
  if (value == INF)
  {
    printf("--,");
  }
  else
  {
    printf("%d,",value);
  }
}

int min(int i, int j)
{
  if (i < j)
  {
    return i;
  }
  return j;
}

int isConnected(int connectivity, int part)
{
  if (connectivity <= part)
  {
    return 1;
  }
  int i, rdn = rand();
  for (i = 0; i < part; i++)
  {
    if ((rdn % connectivity) == i)
    {
      return 1;
    }
  }
  return 0;
}

void makeGraph(int n, int * graph, int max_num, int connectivity, int part)
{
  int i,j,row;
  if (connectivity > 0)
  {
    for (i = 0; i < n; i++)
    {
      row = i * n;
      for (j = 0; j < n; j++)
      {
        if (i != j && isConnected(connectivity,part))
        {
          graph[row + j] = QUAN;
        }
        else
        {
          graph[row + j] = INF;
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
        graph[row + j] = part;
      }
    }
  }
  return;
}

void makeGraphTotal(int n, int * graph, int max_num, int connectivity, int part)
{
  int i,j,row,col,hasConnectionRow,hasConnectionCol,randA,randB;
  if (connectivity > 0)
  {
    for (i = 0; i < n; i++)
    {
      row = i * n;
      for (j = 0; j < n; j++)
      {
        if (i != j && isConnected(connectivity,part))
        {
          graph[row + j] = QUAN;
        }
        else
        {
          graph[row + j] = INF;
        }
      }
    }
    // Now check to make sure there is an edge from and to every node.
    for (i = 0; i < n; i++)
    {
      row = i * n;
      hasConnectionRow = 0;
      hasConnectionCol = 0;
      for (j = 0; j < n; j++)
      {
        col = j * n;
        if(graph[row + j] != INF)
        {
          hasConnectionRow = 1;
        }
        if(graph[col + i] != INF)
        {
          hasConnectionCol = 1;
        }
      }
      if(hasConnectionRow == 0)
      {
        randA = (rand() % n);
        while(randA == i) randA = (rand() % n);
        graph[row + randA] = QUAN;
      }
      if(hasConnectionCol == 0)
      {
        randB = (rand() % n);
        while(randB == i) randB = (rand() % n);
        graph[(randB * n) + i] = QUAN;
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
        graph[row + j] = part;
      }
    }
  }
  return;
}

int addWithInfinity(int A, int B)
{
  if (A == INF)
  {
    return A;
  }
  else if (B == INF)
  {
    return B;
  }
  else
  {
    return (A + B);
  }
}

int getMax(int size)
{
  int max = 2;
  while (max < size)
  {
    max *= 2;
  }
  return max;
}

#endif
