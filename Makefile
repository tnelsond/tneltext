CC = gcc -ansi -pedantic -Wall -std=c89

tneltext : tneltext.c
	$(CC) tneltext.c -o tneltext.run -lreadline -lhistory
