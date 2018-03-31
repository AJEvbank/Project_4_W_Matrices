#ifndef FUNC

#define FUNC

#include "main.h"

vvoid getkRowAndCol(MPI_Comm mcw, int n, int k, int * kthCol, int * kthRow)
{
  int world_rank;
  MPI_Comm_rank(mcw, &world_rank);
  int world_size;
  MPI_Comm_size(mcw, &world_size);

  int level, offset, sliceTag = 0, distTag = 2, rootP = (int)sqrt((double)world_size);
  int slice = n/rootP, start = world_rank * slice, mySlice = slice;
  int processRow = getProcessRow(world_rank,rootP), processCol = getProcessCol(world_rank,rootP);
  int senderStart, senderSlice;
  int * horizontalSlices = (int *)calloc(rootP,sizeof(int));
  int * verticalSlices = (int *)calloc(rootP,sizeof(int));
  MPI_Status status;
  int max = getMax(world_size);
  int kthRowOrigin = getKthRowOrigin(k,rootP,processRow,slice);
  int kthColOrigin = getKthColOrigin(k,rootP,processCol,slice);

  // Initial loop....
  for(level = 2, offset = 1;
    level <= max;
    level = level * 2, offset = offset * 2)
  {

  }

  sliceTag++;
  distTag++;

  // Broadcast of all-clear signal from world_rank 0.
  for (level = max,
       offset = max/2;
       level >= 2;
       level = level / 2,
       offset = offset / 2
       )
   {

  }
  return;
}

int getProcessRow(int world_rank, int rootP)
{
  return (world_rank / rootP);
}

int getProcessCol(int world_rank, int rootP)
{
  return (world_rank % rootP);
}

int getRowMember(int processRow, int rootP, int index)
{
  return ((processRow * rootP) + index);
}

int getColMember(int processCol, int rootP, int index)
{
  return ((rootP * index) + processCol);
}

int getKthRowOrigin(int k, int rootP, int processRow, int slice)
{
  return getRowMember(processRow,rootP,k / slice);
}

int getKthColOrigin(int k, int rootP, int processCol, int slice)
{
  return getColMember(processCol,rootP,k / slice);
}

#endif
