#define MAXSTR 100

typedef struct _Parameters{
   char fastaname[MAXSTR];
   int  kmersize;
   char output[MAXSTR];
} Parameters;

Parameters readParams(int argc, char **argv);
