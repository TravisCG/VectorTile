typedef struct _Fasta {
   char *header;
   char *seq;
} Fasta;

typedef struct _MultiFasta {
  unsigned int num;
  Fasta *fasta;
} MultiFasta;

MultiFasta *readFasta(const char *filename);
void releaseFasta(MultiFasta *mf);