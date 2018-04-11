#ifndef FUNC

#define FUNC

#include "main.h"

void getkRowAndCol(MPI_Comm mcw, int n, int k, int * kthCol, int * kthRow, int * myOriginMatrix)
{
  int world_rank;
  MPI_Comm_rank(mcw, &world_rank);
  int world_size;
  MPI_Comm_size(mcw, &world_size);
  MPI_Status status;

  int i, level, offset, myRowIndex, myColIndex, receiverIndex, senderIndex, receiver, sender;
  int rootP = (int)sqrt((double)world_size);
  int slice = n/rootP, processRow = world_rank / rootP, processCol = world_rank % rootP, tag = 0, originProc = k / slice;
  int max = getMax(rootP);
  int * kthRowPartners = (int *)calloc(rootP,sizeof(int));
  int * kthColPartners = (int *)calloc(rootP,sizeof(int));
  int * kthRowReceived = (int *)calloc(rootP,sizeof(int));
  int * kthColReceived = (int *)calloc(rootP,sizeof(int));

  for(i = 0; i < rootP; i++)
  {
    kthRowPartners[i] = ((processRow * rootP) + i);
    if (kthRowPartners[i] == world_rank) myRowIndex = i;
    kthColPartners[i] = ((rootP * i) + processCol);
    if (kthColPartners[i] == world_rank) myColIndex = i;
  }
  kthRowReceived[originProc] = 1;
  kthColReceived[originProc] = 1;

  if (world_rank == kthRowPartners[originProc])
  {
    for(i = 0; i < slice; i++)
    {
      kthCol[i] = myOriginMatrix[(i * slice) + (k % slice)];
    }
  }

  if (world_rank == kthColPartners[originProc])
  {
    for(i = 0; i < slice; i++)
    {
      kthRow[i] = myOriginMatrix[((k % slice) * slice) + i];
    }
  }

  // Comm loop...
  for(level = 2, offset = 1;
      level <= max;
      level = level * 2, offset = offset * 2)
  {
    // Row ops first.
    if (kthRowReceived[myRowIndex] == 1)
    {
      receiverIndex = (myRowIndex + offset) % rootP;
      receiver = kthRowPartners[receiverIndex];
      if (kthRowReceived[receiverIndex] == 0)
      {
        MPI_Send( kthCol,
                  slice,
                  MPI_INT,
                  receiver,
                  tag,
                  mcw);
      }
    }
    else if (kthRowReceived[myRowIndex] == 0)
    {
      senderIndex = ((myRowIndex - offset) + rootP) % rootP;
      sender = kthRowPartners[senderIndex];
      if (kthRowReceived[senderIndex] == 1)
      {
        MPI_Recv( kthCol,
                  slice,
                  MPI_INT,
                  sender,
                  MPI_ANY_TAG,
                  mcw,
                  &status);
      }
    }
    // Col ops second.
    if (kthColReceived[myColIndex] == 1)
    {
      receiverIndex = (myColIndex + offset) % rootP;
      receiver = kthColPartners[receiverIndex];
      if (kthColReceived[receiverIndex] == 0)
      {
        MPI_Send( kthRow,
                  slice,
                  MPI_INT,
                  receiver,
                  tag,
                  mcw);
      }
    }
    else if (kthColReceived[myColIndex] == 0)
    {
      senderIndex = ((myColIndex - offset) + rootP) % rootP;
      sender = kthColPartners[senderIndex];
      if (kthColReceived[senderIndex] == 1)
      {
        MPI_Recv( kthRow,
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

  free(kthRowPartners);
  free(kthColPartners);
  free(kthRowReceived);
  free(kthColReceived);
  return;
}

int isDiagonalProcess(int world_rank, int rootp)
{
  if ((world_rank % rootp) == (world_rank / rootp))
  {
    return 1;
  }
  else
  {
    return 0;
  }
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

void ParallelizeMatrix(MPI_Comm mcw, int * myMatrix, int slice, int n, int rootP, int * recv)
{
  int world_rank;
  MPI_Comm_rank(mcw, &world_rank);
  int world_size;
  MPI_Comm_size(mcw, &world_size);

  int i,j,m,row, tag = 0, processRow;
  int * buffer = (int *)calloc(slice * slice,sizeof(int));
  MPI_Status status;

  if (world_rank == 0)
  {
    for (i = 0; i < slice; i++)
    {
      row = rootP * slice * i;
      for (j = 0; j < slice; j++)
      {
        recv[row + j] = myMatrix[(i * slice) + j];
      }
    }
  }

  for (i = 1; i < world_size; i++)
  {
    if (world_rank == 0)
    {
      MPI_Recv( buffer,
                slice * slice,
                MPI_INT,
                i,
                tag,
                mcw,
                &status);
      for (m = 0; m < slice; m++)
      {
        processRow = (i / rootP);
        row = (processRow * (slice * slice * rootP)) + (rootP * slice * m) + ((i % rootP) * slice);
        for (j = 0; j < slice; j++)
        {
          recv[row + j] = buffer[(m * slice) + j];
        }
      }
    }
    if (world_rank == i)
    {
      MPI_Send( myMatrix,
                slice * slice,
                MPI_INT,
                0,
                tag,
                mcw);
    }
  }
  free(buffer);
  return;
}
#endif
