CC=gcc
CFLAGS= -Wall -W -pedantic -ggdb
CLIBS= -lm

vectortile: vectortile.o fasta.o
	$(CC) $(CLIBS) *.o -o vectortile
vectortile.o: vectortile.c
	$(CC) $(CFLAGS) vectortile.c -c
fasta.o: fasta.c fasta.h
	$(CC) $(CFLAGS) fasta.c -c

clean:
	rm *.o
	rm vectortile
