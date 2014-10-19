# Makefile for ripdifm
CC = gcc
CFLAGS = -Wall -ggdb
LDFLAGS = -lncursesw -lm

ripdifm : ripdifm.o jsmn/jsmn.o
	$(CC) ripdifm.o jsmn/jsmn.o -o run $(LDFLAGS)

ripdifm.o : ripdifm.c jsmn/jsmn.h
	$(CC) $(CFLAGS) -c ripdifm.c $(LDFLAGS)

clean :
	$(RM) *.o
