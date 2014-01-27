CC=gcc
CFLAGS= -Wall -W -pedantic -ggdb
CLIBS= -lm

vectortile: vectortile.o fasta.o parameters.o multialign.o
	$(CC) *.o $(CLIBS) -o vectortile
vectortile.o: vectortile.c
	$(CC) $(CFLAGS) vectortile.c -c
fasta.o: fasta.c fasta.h
	$(CC) $(CFLAGS) fasta.c -c
parameters.o: parameters.c parameters.h
	$(CC) $(CFLAGS) parameters.c -c
multialign.o: multialign.c multialign.h fasta.h
	$(CC) $(CFLAGS) multialign.c -c

clean:
	rm *.o
	rm vectortile
