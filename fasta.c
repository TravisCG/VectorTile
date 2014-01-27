#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fasta.h"

/**
  readFasta Read multifasta sequence file
*/

MultiFasta *readFasta(const char *filename){
   FILE       *fastafile;
   char       *line = NULL;
   size_t      len;
   ssize_t     readlen;
   MultiFasta *mf;
   unsigned int counter = 0;
   int seqlen;

   mf = malloc(sizeof(MultiFasta));
   if(!mf){
      return NULL;
   }

   mf->num = counter;
   mf->fasta = NULL;

   fastafile = fopen(filename, "r");
   if(!fastafile){
      free(mf);
      return NULL;
   }

   while( (readlen = getline(&line, &len, fastafile)) != -1){
      if(line[0] == '>'){
         /* New header */
         mf->fasta = realloc(mf->fasta, sizeof(Fasta) * (counter + 1));
         mf->fasta[counter].header = malloc(sizeof(char) * (readlen-1));
         strncpy(mf->fasta[counter].header, &line[1], readlen - 2);
         mf->fasta[counter].header[readlen-2] = '\0';
         mf->fasta[counter].seq = NULL;
         seqlen = 0;
         counter++;
      }
      else{
        /* Seq */
        mf->fasta[counter-1].seq = realloc(mf->fasta[counter-1].seq, seqlen + readlen);
        strncpy(&mf->fasta[counter-1].seq[seqlen], line, readlen);
        mf->fasta[counter-1].seq[seqlen + readlen - 1] = '\0';
        seqlen += readlen - 1;
      }
   }

   mf->num = counter;
   free(line);
   fclose(fastafile);

   return(mf);
}

/**
  releaseFasta Release MultiFasta memory
*/

void releaseFasta(MultiFasta *mf){
   unsigned int i;

   for(i = 0; i < mf->num; i++){
      free(mf->fasta[i].header);
      free(mf->fasta[i].seq);
   }

   free(mf->fasta);
   free(mf);
}

/**
  printFasta Print MultiFasta records to standard output
*/

void printFasta(MultiFasta *mf){
  unsigned int i;

  for(i = 0; i < mf->num; i++){
     printf(">%s\n%s\n", mf->fasta[i].header, mf->fasta[i].seq);
  }
}
