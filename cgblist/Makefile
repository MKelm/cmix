# Makefile for cgblist
CC = gcc
CFLAGS = -Wall -ggdb
LDFLAGS =

chanoi : cgblist.o
	$(CC) cgblist.o -o run $(LDFLAGS)

cgblist.o : cgblist.c
	$(CC) $(CFLAGS) -c cgblist.c $(LDFLAGS)

clean :
	$(RM) *.o
