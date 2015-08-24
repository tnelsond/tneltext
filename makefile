CC = gcc -ansi -pedantic -Wall -std=c99
LIBS = -lreadline -lhistory

game.run : example.c tneltext.o
	$(CC) example.c tneltext.o -o game.run $(LIBS)

tneltext.o : tneltext.c tneltext.h
	$(CC) tneltext.c -c
