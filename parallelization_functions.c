#ifndef FUNC

#define FUNC

#include "main.h"

void Reduce_Bcast_Distances(MPI_Comm mcw, int * dist, int n)
{
  int world_rank;
  MPI_Comm_rank(mcw, &world_rank);
  int world_size;
  MPI_Comm_size(mcw, &world_size);

  int level, offset, sliceTag = 0, distTag = 2, slice = n/world_size, start = world_rank * slice, mySlice = slice;
  int senderStart, senderSlice;
  MPI_Status status;
  int max = getMax(world_size);

  // Initial loop....
  for(level = 2, offset = 1;
    level <= max;
    level = level * 2, offset = offset * 2)
  {
      if((world_rank % level) == offset)
      {
        // send slice size first, then slice
        MPI_Send(&mySlice, 1, MPI_INT,
          world_rank - offset, sliceTag, mcw);

        MPI_Send(&dist[start], mySlice, MPI_INT,
          world_rank - offset, distTag, mcw);
      }
      else if((world_rank % level) == 0)
      {
        // receive iff the sender actually exists
        if ((world_rank + offset) < world_size)
        {
          senderStart = (world_rank + offset) * slice;
          // first receive senderSlice, then slice

          MPI_Recv(&senderSlice, 1, MPI_INT,
            world_rank + offset, sliceTag, mcw, &status);

          MPI_Recv(&dist[senderStart], senderSlice, MPI_INT,
            world_rank + offset, distTag, mcw, &status);

          mySlice += senderSlice;
        }
      }
      else {
        continue;
      }
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
     if ((world_rank % level) == 0)
     {
       // Process is a sender and must relay its all-clear to its recipient,
       // but only if the recipient actually exists.
       if ((world_rank + offset) < world_size)
       {
         MPI_Send(dist,n,MPI_INT,world_rank + offset,sliceTag,mcw);
       }
     }
     else if ((world_rank % level) == offset)
     {
       // Process is a receiver and must wait for a check-in from its sender.
       MPI_Recv(dist,n,MPI_INT,world_rank - offset,sliceTag,mcw,&status);
     }
     else
     {
       // Process is neither a sender nor a receiver on this iteration.
       continue;
     }
  }
  return;
}

int Reduce_Bcast_J(MPI_Comm mcw, int localMinimum, int * dist, int n)
{
  int world_rank;
  MPI_Comm_rank(mcw, &world_rank);
  int world_size;
  MPI_Comm_size(mcw, &world_size);

  int level, offset, tag = 0, currentLocalMinimum = localMinimum, sendersMinimum;
  MPI_Status status;
  int max = getMax(world_size);

  // Initial loop....
  for(level = 2, offset = 1;
    level <= max;
    level = level * 2, offset = offset * 2)
  {
      if((world_rank % level) == offset)
      {
        // send local minimum
        MPI_Send(&currentLocalMinimum, 1, MPI_INT,
          world_rank - offset, tag, mcw);
      }
      else if((world_rank % level) == 0)
      {
        // receive iff the sender actually exists
        if ((world_rank + offset) < world_size)
        {
          // receive senders minimum and compare with local minimum
          // keep the smaller
          MPI_Recv(&sendersMinimum, 1, MPI_INT,
            world_rank + offset, tag, mcw, &status);
          if (sendersMinimum != -1 && currentLocalMinimum != -1)
          {
            if (dist[sendersMinimum] < dist[currentLocalMinimum]) { currentLocalMinimum = sendersMinimum; }
          }
          else if (sendersMinimum != -1 && currentLocalMinimum == -1)
          {
            currentLocalMinimum = sendersMinimum;
          }
          else
          {
            //Do nothing.
          }
        }
      }
      else {
        continue;
      }
  }

  tag++;

  // Broadcast of all-clear signal from world_rank 0.
  for (level = max,
       offset = max/2;
       level >= 2;
       level = level / 2,
       offset = offset / 2
       )
   {
     if ((world_rank % level) == 0)
     {
       // Process is a sender and must relay its all-clear to its recipient,
       // but only if the recipient actually exists.
       if ((world_rank + offset) < world_size)
       {
         MPI_Send(&currentLocalMinimum, 1, MPI_INT, world_rank + offset, tag, mcw);
       }
     }
     else if ((world_rank % level) == offset)
     {
       // Process is a receiver and must wait for a check-in from its sender.
       MPI_Recv(&currentLocalMinimum, 1, MPI_INT, world_rank - offset, tag, mcw, &status);
     }
     else
     {
       // Process is neither a sender nor a receiver on this iteration.
       continue;
     }
  }
  return currentLocalMinimum;
}


#endif
