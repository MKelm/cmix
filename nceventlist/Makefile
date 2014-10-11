# Makefile for nceventlist
CC = gcc
CFLAGS = -Wall -ggdb
LDFLAGS = -lncursesw -lm

nceventlist : nceventlist.o display.o phrases.o
	$(CC) nceventlist.o display.o phrases.o -o run $(LDFLAGS)

nceventlist.o : nceventlist.c nceventlist.h display.h phrases.h
	$(CC) $(CFLAGS) -c nceventlist.c

display.o : display.c display.h phrases.h
	$(CC) $(CFLAGS) -c display.c

phrases.o : phrases.c phrases.h
	$(CC) $(CFLAGS) -c phrases.c

clean :
	$(RM) *.o