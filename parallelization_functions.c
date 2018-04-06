#ifndef FUNC

#define FUNC

#include "main.h"

void getkRowAndCol(MPI_Comm mcw, int n, int k, int * kthCol, int * kthRow)
{
  int world_rank;
  MPI_Comm_rank(mcw, &world_rank);
  int world_size;
  MPI_Comm_size(mcw, &world_size);

  int i, level, offset, rootP = (int)sqrt((double)world_size), tag = 0;
  int slice = n/rootP, receiver, sender;
  int processRow = getProcessRow(world_rank,rootP), processCol = getProcessCol(world_rank,rootP);
  int myRowIndex, myColIndex;
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
    if (kthRowPartners[i] == world_rank) myRowIndex = kthRowPartners[i];
    kthColPartners[i] = getColMember(processCol,rootP,i);
    if (kthColPartners[i] == world_rank) myColIndex = kthColPartners[i];
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
      receiver = kthRowPartners[(myRowIndex + offset) % rootP];
      if (kthRowReceived[receiver] == 0)
      {
        MPI_Send(kthRow,
                  slice,
                  MPI_INT,
                  receiver,
                  tag,
                  mcw);
      }
    }
    else if (kthRowReceived[world_rank] == 0)
    {
      sender = kthRowPartners[((myRowIndex - offset) + rootP) % rootP];
      if (kthRowReceived[sender] == 1)
      {
        MPI_Recv(kthRow,
                  slice,
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
      receiver = kthColPartners[(myColIndex + offset) % rootP];
      if (kthColReceived[receiver] == 0)
      {
        MPI_Send(&kthCol,
                  slice,
                  MPI_INT,
                  receiver,
                  tag,
                  mcw);
      }
    }
    else if (kthColReceived[world_rank] == 0)
    {
      sender = kthColPartners[((myColIndex - offset) + rootP) % rootP];
      if (kthColReceived[sender] == 1)
      {
        MPI_Recv(&kthCol,
                  slice,
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

  // free(kthRowPartners);
  // free(kthColPartners);
  // free(kthRowReceived);
  // free(kthColReceived);
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
  for (i = 0; i < rootP; i++) { copy[i] = receivedArray[i]; }
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

void ParallelizeMatrix(MPI_Comm mcw, int * myMatrix, int slice, int * recv)
{
  int world_rank;
  MPI_Comm_rank(mcw, &world_rank);
  int world_size;
  MPI_Comm_size(mcw, &world_size);

  int * recvr;
  if (world_rank == 0)
  {
    recvr = recv;
  }
  else
  {
    recvr = NULL;
  }

  MPI_Gather(
    myMatrix,
    slice * slice,
    MPI_INT,
    recvr,
    slice * slice,
    MPI_INT,
    0,
    mcw);

  return;
}
#endif
