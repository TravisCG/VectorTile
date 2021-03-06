#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "multialign.h"
#include "fasta.h"

/*FIXME right now this is the most time consuming routine */
unsigned long calcHash(const char *seq, unsigned int start, int size){
   int i;
   char code;
   unsigned long result = 0;

   for(i = 0; i < size; i++){
      switch(seq[start + i]){
         case'A':
            code = 0;
            break;
         case'T':
            code = 1;
            break;
         case'G':
            code = 2;
            break;
         case'C':
            code = 3;
            break;
         default:
            fprintf(stderr, "Unsupported nucleotide: %c\n", seq[start + i]);
            code = 0;
      }

      result += code * pow(4, i);
   }

   return(result);
}

/**
  Dynamic programming algorithm to find diagonals
*/
void dotplot(int *matrix, const char *seq1, size_t len1, const char *seq2, size_t len2, int ks){
   unsigned int i, j;
   unsigned long hash1, hash2;
   int k, d;

   for(i = 0; i < len1 - ks; i++){
      hash1 = calcHash(seq1, i, ks);
      hash2 = calcHash(seq2, 0, ks);
      if(hash1 == hash2){
         matrix[i * len2] = 1;
      }
      else{
         matrix[i * len2] = 0;
      }
   }

   for(j = 0; j < len2 - ks; j++){
      hash1 = calcHash(seq1, 0, ks);
      hash2 = calcHash(seq2, j, ks);
      if(hash1 == hash2){
         matrix[j] = 1;
      }
      else{
         matrix[j] = 0;
      }
   }

   for(i = 1; i < len1 - ks; i++){
      hash1 = calcHash(seq1, i, ks);
      for(j = 1; j < len2 - ks; j++){
         hash2 = calcHash(seq2, j, ks);
         if(hash1 == hash2){
            k = 1;
         }
         else{
            k = 0;
         }

         d = matrix[(i - 1) * len2 + (j - 1)];
         matrix[i * len2 + j] = k * (k + d); /* diagonal increased by one or set zero */
      }
   }

}

/**
  Locate and get the size of diagonals
*/
void finddiag(const int *matrix, size_t len1, size_t len2, int ks, int mindiaglen){
   unsigned int i, j;
   int diaglen;

   for(i = 0; i < len1 - ks - 1; i++){
      for(j = 0; j < len2 - ks - 1; j++){
         if( matrix[i * len2 + j] > matrix[(i+1) * len2 + j + 1]){
            /* End of a diagonal */
            diaglen = matrix[i * len2 + j];
            if(diaglen > mindiaglen){
               /* Position vote */
               plasmidst.ps[index1].stat[i - diaglen + 1]++;
               plasmidst.ps[index2].stat[j - diaglen + 1]++;
            }
         }
      }
   }
}

void diagonal(const char *seq1, const char *seq2, int kmersize, int mindiaglen){
   int *matrix; /*FIXME the size of the diagonals limited by the data type*/
   size_t len1, len2;

   len1 = strlen(seq1);
   len2 = strlen(seq2);

   /* Allocate dotplot matrix */
   matrix = malloc(sizeof(int) * len1 * len2);
   if(matrix == NULL){
      fprintf(stderr, "ERROR Skip, because no enough memory\n");
      return;
   }

   /* Fill the dotplot matrix */
   dotplot(matrix, seq1, len1, seq2, len2, kmersize);

   /* Find long diagonals */
   finddiag(matrix, len1, len2, kmersize, mindiaglen);

   free(matrix);
}

/**
  bestpos Find the extreme value position
  If the postion contains more than 2*sddev diagonals it returns it
*/
int calcbestpos(PosStat ps){
   int i;
   int best = 0;
   double mean;
   double sum;
   double sd;
   int max = 0;
   int maxpos = 0;

   /* Calculate mean and max value */
   sum = 0.0;
   for(i = 0; i < ps.numpos; i++){
      sum += ps.stat[i];
      if(ps.stat[i] > max){
         max = ps.stat[i];
         maxpos = i;
      }
   }
   mean = sum / (double)ps.numpos;

   /* Calculate std deviation */
   sum = 0.0;
   for(i = 0; i < ps.numpos; i++){
      sum += (ps.stat[i] - mean) * (ps.stat[i] - mean);
   }
   sd = sqrt(sum / (double)ps.numpos);

   if(max > sd * 2.0){
      best = maxpos;
   }

   return(best);
}

void rotateseq(Fasta *fasta, int bestpos){
   char *buffer;
   size_t len;

   len = strlen(fasta->seq);

   buffer = malloc(sizeof(char) * bestpos);
   if(buffer == NULL){
      fprintf(stderr, "Rotation step missing from %s\n", fasta->header);
      return;
   }
   printf("%s tiled %d\n", fasta->header, bestpos);
   strncpy(buffer, fasta->seq, bestpos);
   memmove(fasta->seq, fasta->seq+bestpos, len - bestpos);
   strncpy(&fasta->seq[len - bestpos], buffer, bestpos);

   free(buffer);
}

void tileplasmids(MultiFasta *mf){
   unsigned int i;
   int bestpos;

   for(i = 0; i < mf->num; i++){
      bestpos = calcbestpos(plasmidst.ps[i]);
      if(bestpos > 0){
         rotateseq(&mf->fasta[i], bestpos);
      }
   }
}

void multialign(MultiFasta *mf, int kmersize, int mindiaglen){
   unsigned int i,j;

   /* Allocate memory for plasmid statistics */
   plasmidst.ps = malloc(sizeof(PosStat) * mf->num);
   if(plasmidst.ps == NULL){
      fprintf(stderr, "Memory not enough for this number of plasmids");
      return;
   }
   plasmidst.plasmidnum = mf->num;

   for(i = 0; i < mf->num; i++){
      j = strlen(mf->fasta[i].seq);
      plasmidst.ps[i].stat = calloc(j, sizeof(int));
      plasmidst.ps[i].numpos = j;
   }

   /* Calculate diagonals and vote for tile positions */
   for(i = 0; i < mf->num - 1; i++){
      for(j = i+1; j < mf->num; j++){
         printf("*******\n%s VS. %s\n*******\n", mf->fasta[i].header, mf->fasta[j].header);
         index1 = i;
         index2 = j;
         diagonal(mf->fasta[i].seq, mf->fasta[j].seq, kmersize, mindiaglen);
      }
   }

   /* Select best positions from plasmid statistics */
   tileplasmids(mf); 

   /* Release resources */
   for(i = 0; i < mf->num; i++){
      free(plasmidst.ps[i].stat);
   }

   free(plasmidst.ps);
}
