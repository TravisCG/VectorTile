#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parameters.h"

Parameters readParams(int argc, char **argv){
   Parameters p;
   int i;
   char error = 0;
   char fastaisset = 0;

   /* Default parameters*/
   p.kmersize = 4;
   p.mindiagsize = 10;

   /* Parse command line parameters */
   for(i = 1; i < argc; i++){
      if(!strcmp(argv[i], "-kmer")){
         p.kmersize = atoi(argv[i+1]);
      }
      if(!strcmp(argv[i], "-fasta")){
         fastaisset = 1;
         strncpy(p.fastaname, argv[i+1], MAXSTR);
      }
      if(!strcmp(argv[i], "-mindsize")){
         p.mindiagsize = atoi(argv[i+1]);
      }
   }

   /* Checking parameters */
   if(!fastaisset){
      printf("Please set the input fasta name (-fasta)\n");
      error = 1;
   }
   if(p.kmersize < 2 || p.kmersize > 20){
      printf("Kmer size must be between 2 and 20\n");
      error = 1;
   }

   if(error){
      exit(1);
   }

   return(p);
}
