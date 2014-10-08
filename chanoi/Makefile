# Makefile for chanoi
CC = gcc
CFLAGS = -Wall -ggdb
LDFLAGS =

chanoi : chanoi.o
	$(CC) chanoi.o -o run $(LDFLAGS)

chanoi.o : chanoi.c
	$(CC) $(CFLAGS) -c chanoi.c $(LDFLAGS)

clean :
	$(RM) *.o
