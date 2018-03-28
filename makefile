all:
	mpicc -Wall main.c main.h functions.c Comm_Line.c -o main -lm
