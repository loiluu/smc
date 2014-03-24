#include "utils.h"
#include "contains.h"
#include "mathlink.h"
#include "config.h"
#include <stdlib.h>
#define DEBUG 0


/*
This function returns the GF of bounds for False = S.contains(s)
*/
void count_not_contains(CONFIGURE *conf, MLINK lp, char* s, bounds* results)
{
   int slen = strlen(s), len;
   int ctable[slen];
   
   compute_correlation_table(s, ctable, slen);   
   if (DEBUG)
      printf("%s %d \n", s, slen);

   int i;
   char* c = malloc(slen*5);
   strcpy(c, " ");
   for (i = 0; i < slen; i ++)
   {
      char tmp[10];
      if (ctable[i] == 1)
      {
         sprintf(tmp, "+X^%d", i);
         strcat(c, tmp);
      }
   }

   if (DEBUG)
      printf("%s \n", c);

   char* expression =  malloc(slen*14 + 30);
   sprintf(expression, "(%s)/(X^%d + (1-%dX)(%s))", c, slen, conf->alphabet_size, c);
   
   char* sresult;
   send_to_mathematica(lp, expression, &sresult);
   (*results).lowerBound = malloc(strlen(sresult)+1);
   strcpy((*results).lowerBound, sresult);
   (*results).upperBound = malloc(strlen(sresult)+1);
   strcpy((*results).upperBound, sresult);
}

/*
This function returns the GF of bounds for True = S.contains(s)
*/
void count_contains(CONFIGURE *conf, MLINK lp, char* s, bounds* results)
{
   int slen = strlen(s), len;
   int ctable[slen];
   
   compute_correlation_table(s, ctable, slen);   
   
   int i;
   char* c = malloc(slen*5+10);
   strcpy(c, " ");
   for (i = 0; i < slen; i ++)
   {
      char tmp[10];
      if (ctable[i] == 1)
      {
         sprintf(tmp, "+X^%d", i);
         strcat(c, tmp);
      }
   }

   char* expression =   malloc(slen*14 + 100);
   sprintf(expression, "1/(1-%dX) - (%s)/(X^%d + (1-%dX)(%s))", conf->alphabet_size, c, slen, conf->alphabet_size, c);
   
   char* sresult;
   send_to_mathematica(lp, expression, &sresult);
   
   copy_bounds_concrete(sresult, sresult, results);
   free(sresult);
   free(expression);
   free(c);
}

/*
Generate GF function for combination of not contains
 */
void compute_from_n_correlation_table(CONFIGURE *conf, MLINK lp, int *ctable[], int *slen, int n, char** lBound)
{
   int i, j;
   char *Cp[n];

   //generate the correlation polynomial of all strings
   for (i = 0; i < n; i ++)
   {
      Cp[i] = malloc(slen[i]*7+10);
      strcpy(Cp[i], "(");
      for (j = 0; j < slen[i]; j++)
      {
         char tmp[10];
         if (ctable[i][j] == 1){
            sprintf(tmp, "+X^%d", j);
            strcat(Cp[i], tmp);
         }
      }
      strcat(Cp[i], ")");      
   }

   //generate the F(x)
   int lenfx = 0;
   for (i = 0; i < n; i++) lenfx += strlen(Cp[i])+2;
   char *fx = (char *)malloc(lenfx);
   strcpy(fx, " ");
   for (i = 0; i < n; i++)
      if (strlen(Cp[i]) > 2)
      {
         char* tmp =  malloc(strlen(Cp[i])+3);
         sprintf(tmp, "%s*", Cp[i]);
         strcat(fx, tmp);
      }
   //remove the last *
   fx[strlen(fx) - 1] = '\0';
   
   if (DEBUG)
      printf("Fx is %s \n", fx);

   //generate D
   int lenD = 0;
   for (i = 0; i < n; i++) lenD += strlen(Cp[i])+10;
   char *D = (char *)malloc(lenD + strlen(fx));
   strcpy(D, "");
   for (i = 0; i < n; i++)
   {
      char* tmp = malloc(strlen(Cp[i]) + 10);
      sprintf(tmp, "X^%d/%s+", slen[i], Cp[i]);
      strcat(D, tmp);
   }
   //remove the last +
   D[strlen(D) - 1] = '\0';
   char* tD = (char *)malloc(strlen(D)+1);
   strcpy(tD, D);
   sprintf(D, "(%s)/(%s)", tD, fx);
   if (DEBUG)
      printf("D is %s \n", D);

   //generate gx
   char *gx = (char *)malloc(strlen(fx) + strlen(D) + 20);
   sprintf(gx, "(%s + %s*(1-%d*X))", D, fx, conf->alphabet_size);
   if (DEBUG)
      printf("Gx is %s \n", gx);
   
   char* query = malloc(strlen(fx) + strlen(gx) + 10);
   sprintf(query, "%s/%s", fx, gx);
   send_to_mathematica(lp, query, lBound);
   // printf("Result %s\n", *lBound);
   
   free(fx);
   free(gx);
   free(D);
   for (i = 0; i < n; i ++) free(Cp[i]);
}

/*
This function returns the GF of bounds for False = S.contains(s_i)
*/
void count_n_not_contains(CONFIGURE *conf, MLINK lp, char** s, int n, bounds* results)
{
   int  i, j;
   // printf ("Parameters %d\n", n);
   // for (j = 0; j < n; j++) printf("String %d is %s \n", j, s[j]);

   int slen[n];
   int *ctable[n];  

   for (i = 0; i < n; i++)
   {
      slen[i] = strlen(s[i]);
      ctable[i] = malloc(sizeof(int) * slen[i]);
      compute_correlation_table(s[i], ctable[i], slen[i]);
   }
   
   char* lBound;
   char* minupper;
   compute_from_n_correlation_table(conf, lp, ctable, slen, n, &lBound);

   //compute the upper bounds which is MIN(not contains(A_i))
   for (i = 0; i < n; i++){
      char* c = malloc(slen[i]*5+10);
      strcpy(c, " ");
      for (j = 0; j < slen[i]; j ++)
      {
         char tmp[10];
         if (ctable[i][j] == 1)
         {
            sprintf(tmp, "+X^%d", j);
            strcat(c, tmp);
         }
      }

      char* expression =  malloc(slen[i]*14 + 30);
      sprintf(expression, "(%s)/(X^%d + (1-%dX)(%s))", c, slen[i], conf->alphabet_size, c);
      
      char* gfi;
      send_to_mathematica(lp, expression, &gfi);
      if (i > 0){
         char* tmpbest = malloc(strlen(minupper)+1);
         strcpy (tmpbest, minupper);
         MINF(conf, lp, tmpbest, gfi, &minupper);
         free(tmpbest);
      }
      else{
         minupper = malloc(strlen(gfi)+1);
         strcpy (minupper, gfi);
      }
   }
   
   copy_bounds_concrete(lBound, minupper, results);
   // printf("Result of combination %s\n", lBound);
   free(minupper);
   free(lBound);
   for (i = 0; i < n; i++)
      free(ctable[i]);
}

/*
Build GF for strstr(S, pattern) = position
 */
void count_strstr(CONFIGURE *conf, MLINK lp, char* pattern, int position, bounds* results){
   if (position < 0){
      // printf("%d %s\n", position, pattern);
      count_contains(conf, lp, pattern, results);      
      return;
   }
   int n = strlen(pattern), i, j;
   int ctable[n];
   compute_correlation_table(pattern, ctable, n);
   
   bounds F0;
   char* E;
   count_not_contains(conf, lp, pattern, &F0);
   get_coef(conf, lp, F0.upperBound, position, &E);
   
   for (i = 1; i < n; i++){
      if (i > position) break;
      if (ctable[i] == 1){
            bounds fi;
            char* tmp;
            count_strstr(conf, lp, pattern, position-i, &fi);
            get_coef(conf, lp, fi.upperBound, n+position-i, &tmp);
            char* tmp2 = malloc(strlen(tmp) + strlen(E) + 5);
            sprintf(tmp2, "%s - %s", E, tmp);
            send_to_mathematica(lp, tmp2, &E);
            
            free(tmp);
            free(tmp2);
         }
   }
   char* query =  malloc(strlen(E) + 30);
   sprintf(query, "(%s)X^%d*X^%d/(1-%dX)", E, position, n, conf->alphabet_size);
   char* sresult;
   send_to_mathematica(lp, query, &sresult);
   
   if (DEBUG)
      printf("Result = %s\n", sresult);
   copy_bounds_concrete(sresult, sresult, results);

   free(query);
   free(sresult);
   free(E);
}