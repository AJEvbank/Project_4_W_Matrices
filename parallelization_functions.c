#ifndef FUNC

#define FUNC

#include "main.h"

void getkRowAndCol(MPI_Comm mcw, int n, int k, int * kthCol, int * kthRow, int * myOriginMatrix)
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
  int max = getMax(rootP);
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
  kthRowReceived[ k / slice ] = 1;
  kthColReceived[ k / slice ] = 1;

  if (world_rank == kthRowPartners[ k / slice ])
  {
    for(i = 0; i < slice; i++)
    {
      kthCol[i] = myOriginMatrix[(i * slice) + (k % slice)];
    }
  }

  if (world_rank == kthColPartners[ k / slice ])
  {
    for(i = 0; i < slice; i++)
    {
      kthRow[i] = myOriginMatrix[((k % slice) * slice) + i];
    }
  }

  printf("kthRowReceived on iteration -1 on %d:\n",world_rank);
  printArray(kthRowReceived,rootP);
  printf("\n");
  printf("kthColReceived on iteration -1 on %d:\n",world_rank);
  printArray(kthColReceived,rootP);
  printf("\n");

  // Comm loop...
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
        // MPI_Send( kthCol,
        //           slice,
        //           MPI_INT,
        //           receiver,
        //           tag,
        //           mcw);
      }
    }
    else if (kthRowReceived[world_rank] == 0)
    {
      sender = kthRowPartners[((myRowIndex - offset) + rootP) % rootP];
      if (kthRowReceived[sender] == 1)
      {
        // MPI_Recv( kthCol,
        //           slice,
        //           MPI_INT,
        //           sender,
        //           MPI_ANY_TAG,
        //           mcw,
        //           &status);
      }
    }

    // Col ops second.
    if (kthColReceived[world_rank] == 1)
    {
      receiver = kthColPartners[(myColIndex + offset) % rootP];
      if (kthColReceived[receiver] == 0)
      {
        // MPI_Send( kthRow,
        //           slice,
        //           MPI_INT,
        //           receiver,
        //           tag,
        //           mcw);
      }
    }
    else if (kthColReceived[world_rank] == 0)
    {
      sender = kthColPartners[((myColIndex - offset) + rootP) % rootP];
      if (kthColReceived[sender] == 1)
      {
        // MPI_Recv( kthRow,
        //           slice,
        //           MPI_INT,
        //           sender,
        //           MPI_ANY_TAG,
        //           mcw,
        //           &status);
      }
    }

    loopOperation(offset,level,kthRowReceived,rootP);
    loopOperation(offset,level,kthColReceived,rootP);

    printf("kthRowReceived on offset %d on %d:\n",offset,world_rank);
    printArray(kthRowReceived,rootP);
    printf("\n");
    printf("kthColReceived on offset %d on %d:\n",offset,world_rank);
    printArray(kthColReceived,rootP);
    printf("\n");
  }

  free(kthRowPartners);
  free(kthColPartners);
  free(kthRowReceived);
  free(kthColReceived);
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
        processRow = getProcessRow(i,rootP);
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
