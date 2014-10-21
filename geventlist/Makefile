# Makefile for geventlist
CC = gcc
CFLAGS = -Wall -ggdb
LDFLAGS = `pkg-config --cflags gtk+-2.0` `pkg-config --libs gtk+-2.0`

geventlist : geventlist.o eventlist.o
	$(CC) geventlist.o eventlist.o -o geventlist $(LDFLAGS)

geventlist.o : geventlist.c eventlist.h
	$(CC) $(CFLAGS) -c geventlist.c $(LDFLAGS)

eventlist.o : eventlist.c eventlist.h
	$(CC) $(CFLAGS) -c eventlist.c $(LDFLAGS)

clean :
	$(RM) *.o