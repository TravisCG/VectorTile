#include "fasta.h"

typedef struct _PosStat{
   int numpos;
   int *stat;
} PosStat;

typedef struct _PlasmidStat{
   int plasmidnum;
   PosStat *ps;
} PlasmidStat;

/* Global variables */
PlasmidStat plasmidst;
unsigned int index1;
unsigned int index2;

void multialign(MultiFasta *mf, int ksize, int mindiaglen);
