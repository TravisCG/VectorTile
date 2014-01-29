#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "multialign.h"
#include "fasta.h"

/*FIXME right now this is the most time consuming routine */
/* I use this numbers because GPU can not compare strings */
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
            printf("Unsupported nucleotide: %c\n", seq[start + i]);
            code = 0;
      }

      result += code * pow(4, i);
   }

   return(result);
}

/**
  Dynamic programming algorithm to find diagonals
*/
void dotplot(char *matrix, const char *seq1, size_t len1, const char *seq2, size_t len2, int ks){
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
         matrix[i * len2 + j] = k * (k + d); /* diagonal increased by one or set to zero */
      }
   }

}

/**
  Locate and get the size of diagonals
*/
void finddiag(const char *matrix, size_t len1, size_t len2, int ks, char *id1, char *id2){
   unsigned int i, j;

   for(i = 0; i < len1 - ks - 1; i++){
      for(j = 0; j < len2 - ks - 1; j++){
         if( matrix[i * len2 + j] > matrix[(i+1) * len2 + j + 1]){
            /* End of a diagonal */
            printf("%d\t%d\t%d\t%s\t%s\n", matrix[i * len2 + j], i, j, id1, id2);
         }
      }
   }
}

void diagonal(const char *seq1, const char *seq2, int kmersize, char *id1, char *id2){
   char *matrix;
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
   finddiag(matrix, len1, len2, kmersize, id1, id2);

   free(matrix);
}

void multialign(MultiFasta *mf, int kmersize){
   unsigned int i,j;

   printf("diaglen\tdiagposA\tdiagposB\tidA\tidB\n");

   for(i = 0; i < mf->num - 1; i++){
      for(j = i+1; j < mf->num; j++){
         //printf("*******\n%s VS. %s\n*******\n", mf->fasta[i].header, mf->fasta[j].header);
         diagonal(mf->fasta[i].seq, mf->fasta[j].seq, kmersize, mf->fasta[i].header, mf->fasta[i].header);
      }
   }
}
