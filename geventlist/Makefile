# Makefile for geventlist
ARCH := $(shell uname -m)
CC = gcc
CFLAGS = -Wall -ggdb
LDFLAGS = `pkg-config --cflags gtk+-2.0` `pkg-config --libs gtk+-2.0`

geventlist : geventlist.o eventlist.o phrases.o
	$(CC) geventlist.o eventlist.o phrases.o -o geventlist.$(ARCH) $(LDFLAGS)

geventlist.o : geventlist.c eventlist.h phrases.h
	$(CC) $(CFLAGS) -c geventlist.c $(LDFLAGS)

eventlist.o : eventlist.c eventlist.h
	$(CC) $(CFLAGS) -c eventlist.c $(LDFLAGS)

phrases.o : phrases.c phrases.h
	$(CC) $(CFLAGS) -c phrases.c $(LDFLAGS)

clean :
	$(RM) *.o