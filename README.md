# Project_4_W_Matrices

Our algorithm first runs the parallel version on all the processes and then
runs the sequential version on process 0 and compares the results of the two
executions to make sure the result is the same.

In order to parallelize the algorithm, we had to transmit fragments of the kth
row and kth column to the appropriate processes. We do this using a loop running
log of square root P iterations where P is the number of processes.

On each iteration of the loop a given process will transmit or receive either or
both of the kth row and kth column, if it or its communication partner has the
required fragment.

On a given iteration of k, a process with interval [start,end] of n requires the
interval [start,end] of the kth row and the kth column. Processes are
conceptualized as laid out in a square. Processes on the same row must all
have the same fragment of the kth column. Processes in the same column must
all have the same fragment of the kth row.

The following variables play the given role in the function getkRowAndCol:

level: a loop interation variable, while level <= max, iterate
offset: a loop variable used to determine sender/receiver index
myRowIndex: index of the process on its process row group
myColIndex: index of the process on its process column group
receiverIndex: index of the process which will receive a transmitted
  fragment within a given group of processes
senderIndex: index of the process which will send a transmitted
  fragment within a given group of processes
receiver: world rank of the process which will receive a transmitted
  fragment within a given group of processes
sender: world rank of the process which will send a transmitted
  fragment within a given group of processes
rootP: square root of the number of processes
slice: n / rootP or the length of one side of the portion of the matrix
  stored on any process
processRow: range [0,rootP] the number of the row group of processes with
  which this process is associated
processCol: range [0,rootP] the number of the column group of processes
  with which this process is associated
tag: a message tag variable
originProc: at the beginning of the transmission on the kth loop, this is
  the index of the process which has either or both of the kth row or column
max: this is set to ceiling of log of rootP
kthRowPartners: [rootP] stores the world ranks of the processes on the same
  row as the current process
kthColPartners: [rootP] stores the world ranks of the processes on the same
  column as the current process
kthRowReceived: [rootP] This array records which processes have the kth
  column fragment (=1) and which processes do not have it (=0)
kthColReceived: [rootP] This array records which processes have the kth
  row fragment (=1) and which processes do not have it (=0)

The function loopOperation updates the kthColReceived and kthRowReceived
arrays by emulating the calculation of which processes will send and which
processes will receive.

When two processes communicate,
receiver = myRowIndex/myColIndex + offset
receiver = myRowIndex/myColIndex - offset
and the indices are 'wrapped' to stay within rootP.
Since offset doubles on each iteration, the number of sender/receiver pairs
doubles unless P is not a power of two, in which case the number of pairs
will cover only the remainder of processes which do not have the target
fragment yet.

Processes only transmit to recipients that do not already have the desired
fragment. On each iteration, a process may communicate with one of its row
partners and one of its column partners.
