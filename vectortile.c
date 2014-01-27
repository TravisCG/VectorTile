#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "fasta.h"

int main(int argc, char **argv){
  MultiFasta *fasta;

  fasta = readFasta(argv[1]);
  printFasta(fasta);
  releaseFasta(fasta);

  return EXIT_SUCCESS;
}
