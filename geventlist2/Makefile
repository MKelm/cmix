# Makefile for geventlist
ARCH := $(shell uname -m)
CC = gcc
CFLAGS = -Wall -ggdb
LDFLAGS = -rdynamic `pkg-config --cflags gtk+-2.0` `pkg-config --libs gtk+-2.0`

geventlist : geventlist.o
	$(CC) geventlist.o -o geventlist.$(ARCH) $(LDFLAGS)

geventlist.o : geventlist.c
	$(CC) $(CFLAGS) -c geventlist.c $(LDFLAGS)

clean :
	$(RM) *.o