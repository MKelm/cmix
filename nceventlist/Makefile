# Makefile for nceventlist
CC = gcc
CFLAGS = -Wall -ggdb
LDFLAGS = -lncursesw

nceventlist : nceventlist.o display.o
	$(CC) nceventlist.o display.o -o run $(LDFLAGS)

nceventlist.o : nceventlist.c nceventlist.h display.h
	$(CC) $(CFLAGS) -c nceventlist.c

display.o : display.c display.h
	$(CC) $(CFLAGS) -c display.c

clean :
	$(RM) *.o