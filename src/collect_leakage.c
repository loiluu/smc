//compile as gcc -o collect_leakage collect_leakage.c -lML64i3 -lm -lpthread -lrt -lstdc++
#include "mathlink.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "utils.h"

void get_leakage(MLINK lp, char* gf, char** result, int degGf){
   char* query = malloc(strlen(gf) + 100);   
   sprintf(query, "e[X_] = Normal[Series[%s,  {X, 0, %d}]];", gf, degGf+1);
   MLPutFunction(lp, "EnterTextPacket", 1);
   MLPutString(lp, query);
   MLEndPacket(lp);

   char* query_result = malloc(10000);
   sprintf(query_result, "result = Coefficient[e[X], X, %d];", degGf);
   MLPutFunction(lp, "EnterTextPacket", 1);
   MLPutString(lp, query_result);
   MLEndPacket(lp);

   sprintf(query_result, "%d - If[result > 0, Log[2., result], 0]", 8*degGf);
   MLPutFunction(lp, "EnterTextPacket", 1);
   MLPutString(lp, query_result);
   MLEndPacket(lp);

   MLPutFunction(lp, "EvaluatePacket", 1);
   MLPutFunction(lp, "ToExpression", 1);
   MLPutString(lp, "ToString[%, InputForm]");
   MLEndPacket(lp);
   int pkt;
   /* skip any packets before the first ReturnPacket */
   while ( (pkt = MLNextPacket( lp), pkt) && pkt != RETURNPKT)
   {
      MLNewPacket(lp);
      if ( MLError(lp)) error( lp);
   }

   const char* str;
   if(! MLGetString(lp, &str))        
         printf("%s\n", "Unable to read string from Mathamatica");
   else{  
      *result = malloc(strlen(str)+1);
      strcpy(*result, str);         
      MLReleaseString(lp, str);
   }   
   free(query);
   free(query_result);
}

//get the range of leakage from output file
int main(int argc, char const *argv[])
{
   if (argc < 2){
      printf("Usage: ./collect_leakage file_name\n");
      return 0;
   }
   
   CONFIGURE conf;
   read_configuration(&conf);
   int degGf;
   if (argc == 3)
      degGf = atoi(argv[2]);
   else
      degGf = conf.max_gf;
   // printf("%d \n", degGf);
   FILE *file;
   //get the length bounds
   file = fopen(argv[1], "r");

   size_t len=0;
   ssize_t read;
   char* name;
   read = getline(&name, &len, file);
   char* lowerbound;
   char* upperbound;
   size_t lenl=0;
   read = getline(&lowerbound, &lenl, file);
   size_t lenu=0;
   read = getline(&upperbound, &lenu, file);   
   fclose(file);

   //get the main part
   char* ugf = strtok(upperbound, ":");
   ugf = strtok(NULL, ":");   

   char* lgf = strtok(lowerbound, ":");
   lgf = strtok(NULL, ":");  

   //open connection
   MLINK lp;
   MLEnvironment env;
   env = MLInitialize(NULL);
   if (env == NULL) return;
   int argcs = 4;
   char *argvs[5] = {"-linkmode", "launch", "-linkname", "math -mathlink", NULL};
   lp = MLOpen(argcs, argvs);
   if (lp == NULL) return;

   char* min_leakage;
   char* max_leakage;

   get_leakage(lp, ugf, &min_leakage, degGf);
   get_leakage(lp, lgf, &max_leakage, degGf);

   //close Mathematica pipe
   MLPutFunction(lp, "Exit", 0L);
   MLClose(lp);
   MLDeinitialize(env);

   printf("Min leakage %s bit(s)\nMax leakage %s bit(s)\n", min_leakage, max_leakage);
   free(min_leakage);
   free(max_leakage);
   free(upperbound);
   free(lowerbound);
}
