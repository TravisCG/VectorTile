#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "fasta.h"
#include "parameters.h"
#include "multialign.h"

int main(int argc, char **argv){
  MultiFasta *fasta;
  Parameters  param;

  param = readParams(argc, argv);

  fasta = readFasta(param.fastaname);
  multialign(fasta, param.kmersize, param.mindiagsize);
  printFasta(fasta);
  releaseFasta(fasta);

  return EXIT_SUCCESS;
}
