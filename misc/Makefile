# Makefile for geventlist
ARCH := $(shell uname -m)
CC = gcc
CFLAGS = -Wall -ggdb
LDFLAGS = -rdynamic `pkg-config --cflags gtk+-2.0` `pkg-config --libs gtk+-2.0`

gmultilang : gmultilang.o
	$(CC) gmultilang.o -o gmultilang.$(ARCH) $(LDFLAGS)

gmultilang.o : gmultilang.c
	$(CC) $(CFLAGS) -c gmultilang.c $(LDFLAGS)

clean :
	$(RM) *.o