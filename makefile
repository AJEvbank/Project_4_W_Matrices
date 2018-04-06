all:
	mpicc -Wall main.c main.h functions.c Comm_Line.c parallelization_functions.c -o main -lm
