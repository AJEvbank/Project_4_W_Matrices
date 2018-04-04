#ifndef FUNC

#define FUNC

#include "main.h"

void getkRowAndCol(MPI_Comm mcw, int n, int k, int * kthCol, int * kthRow)
{
  int world_rank;
  MPI_Comm_rank(mcw, &world_rank);
  int world_size;
  MPI_Comm_size(mcw, &world_size);

  int i,level, offset, sliceTag = 0, distTag = 2, rootP = (int)sqrt((double)world_size);
  int slice = n/rootP, start = world_rank * slice, mySlice = slice, receiver, sender;
  int processRow = getProcessRow(world_rank,rootP), processCol = getProcessCol(world_rank,rootP);
  int senderStart, senderSlice;
  int * horizontalSlices = (int *)calloc(rootP,sizeof(int));
  int * verticalSlices = (int *)calloc(rootP,sizeof(int));
  MPI_Status status;
  int max = getMax(world_size);
  int kthRowOrigin = getKthRowOrigin(k,rootP,processRow,slice);
  int kthColOrigin = getKthColOrigin(k,rootP,processCol,slice);
  int * kthRowPartners = (int *)calloc(rootP,sizeof(int));
  int * kthColPartners = (int *)calloc(rootP,sizeof(int));
  int * kthRowReceived = (int *)calloc(rootP,sizeof(int));
  int * kthColReceived = (int *)calloc(rootP,sizeof(int));

  for(i = 0; i < rootP; i++)
  {
    kthRowPartners[i] = getRowMember(processRow,rootP,i);
    kthColPartners[i] = getColMember(processCol,rootP,i);
  }
  kthRowReceived[kthRowOrigin] = 1;
  kthColReceived[kthColOrigin] = 1;

  // Initial loop....
  for(level = 2, offset = 1;
      level <= max;
      level = level * 2, offset = offset * 2)
  {
    // Row ops first.
    if (kthRowReceived[world_rank] == 1)
    {
      receiver = kthRowPartners[(world_rank + offset) % rootP];
      if (kthRowReceived[receiver] == 0)
      {
        MPI_Send(kthRow,
                  rootP,
                  MPI_INT,
                  receiver,
                  MPI_ANY_TAG,
                  mcw);
      }
    }
    else if (kthRowReceived[world_rank] == 0)
    {
      sender = kthRowPartners[(world_rank - offset) % rootP];
      if (kthRowReceived[sender] == 1)
      {
        MPI_Recv(kthRow,
                  rootP,
                  MPI_INT,
                  sender,
                  MPI_ANY_TAG,
                  mcw,
                  &status);
      }
    }


    // Col ops second.
    if (kthColReceived[world_rank] == 1)
    {
      receiver = kthColPartners[(world_rank + offset) % rootP];
      if (kthColReceived[receiver] == 0)
      {
        MPI_Send(&kthCol,
                  rootP,
                  MPI_INT,
                  receiver,
                  MPI_ANY_TAG,
                  mcw);
      }
    }
    else if (kthColReceived[world_rank] == 0)
    {
      sender = kthColPartners[(world_rank - offset) % rootP];
      if (kthColReceived[sender] == 1)
      {
        MPI_Recv(&kthCol,
                  rootP,
                  MPI_INT,
                  sender,
                  MPI_ANY_TAG,
                  mcw,
                  &status);
      }
    }
    loopOperation(offset,level,kthRowReceived,rootP);
    loopOperation(offset,level,kthColReceived,rootP);
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

void loopOperation(int offset, int level, int * receivedArray, int rootP)
{
  int i;
  int * copy = (int *)calloc(rootP,sizeof(int));
  for (i = 0; i < rootP; i++) { copy[i]=receivedArray[i] }
  for (i = 0; i < rootP; i++)
  {
    if (copy[i] == 1)
    {
      receivedArray[(i + offset) % rootP] = copy[i];
    }
  }
  free(copy);
  return;
}

void ParallelizeMatrix(mcw, int * myMatrix, int n)
{
  int world_rank;
  MPI_Comm_rank(mcw, &world_rank);
  int world_size;
  MPI_Comm_size(mcw, &world_size);
  // Where to plug it in?
  int level, offset;
  int max = getMax(world_size);
  int * buffer = (int *)calloc(n * n,sizeof(int));
  for(level = 2, offset = 1;
      level <= max;
      level = level * 2, offset = offset * 2)
  {
    if(world_rank % level == offset)
    {
      MPI_Send(&myMatrix,
                rootP,
                MPI_INT,
                receiver,
                MPI_ANY_TAG,
                mcw);
    }
  }
  return;
}
#endif
