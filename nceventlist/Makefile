# Makefile for nceventlist
CC = gcc
CFLAGS = -Wall -ggdb
LDFLAGS = -lncursesw

nceventlist : nceventlist.o
	$(CC) nceventlist.o -o run $(LDFLAGS)

nceventlist.o : nceventlist.c nceventlist.h
	$(CC) $(CFLAGS) -c nceventlist.c

clean :
	$(RM) *.o