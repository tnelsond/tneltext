CC = gcc -ansi -pedantic -Wall -std=c99
LIBS = -lreadline -lhistory

tneltext : tneltext.c
	$(CC) tneltext.c -o tneltext.run $(LIBS)
