# Makefile-Datei für Listing ncgblist
CC = gcc
CFLAGS = -Wall -ggdb
LDFLAGS = -lncursesw

ncgblist : ncgblist.o display.o
	$(CC) ncgblist.o display.o -o run $(LDFLAGS)

ncgblist.o : ncgblist.c display.h
	$(CC) $(CFLAGS) -c ncgblist.c

display.o : display.c display.h
	$(CC) $(CFLAGS) -c display.c

clean :
	$(RM) *.o