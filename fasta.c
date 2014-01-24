#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fasta.h"

MultiFasta *readFasta(const char *filename){
   FILE       *fastafile;
   char       *line;
   size_t      len;
   ssize_t     readlen;
   MultiFasta *mf;
   unsigned int counter = 0;

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
         mf->fasta[counter].header = malloc(sizeof(char) * readlen);
         strncpy(mf->fasta[counter].header, &line[1], readlen - 1);
         mf->fasta[counter].header[readlen] = '\0';
         counter++;
      }
      else{
        /* Seq */
      }
   }

   free(line);
   fclose(fastafile);

   return(mf);
}

void releaseFasta(MultiFasta *mf){
   unsigned int i;

   for(i = 0; i < mf->num; i++){
      free(mf->fasta[i].header);
      free(mf->fasta[i].seq);
   }

   free(mf->fasta);
   free(mf);
}
