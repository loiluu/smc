#include "utils.h"
#include "mathlink.h"
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "list.h"
#include "config.h"
#include <errno.h>
#define DEBUG 0


/*
initialize bounds
 */
void initialize(CONFIGURE *conf, bounds* b, int n){
   b->name =  malloc(MAX_PATTERN_LEN);
   b->nc = 0;
   b->nm = 0;
   b->n_and_match = 0;
   b->lenL = 0;
   b->lenU = conf->max_gf;
   b->ne = 0;
   
   b->content_opr = 0;   

   b->concat_list = (intList*)malloc(sizeof(intList));
   b->concat_list->next = NULL;
   b->nconcat = 0;
   b->zero_lower_bound = 0;
   

   b->lowerBound = NULL;
   b->upperBound = NULL;
   b->not_equal = (stringList*)malloc(sizeof(stringList));
   b->not_equal->next = NULL;
   b->not_match = (stringList*)malloc(sizeof(stringList));
   b->not_match->next = NULL;
   b->not_contains = (stringList*)malloc(sizeof(stringList));
   b->not_contains->next = NULL;
   b->and_match = (stringList*)malloc(sizeof(stringList));
   b->and_match->next = NULL;
}

/*
Send request expression, using Mathematica to normalize the expression
 */
void send_to_mathematica(MLINK lp, char *expression, char** result)
{
   char* query = malloc(strlen(expression) + 20); 

   if (DEBUG)
      printf("Expression %s \n", expression);
   sprintf(query, "Expand[%s];", expression);
   MLPutFunction(lp, "EnterTextPacket", 1);
   MLPutString(lp, query);
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
   
   *result = malloc(strlen(str)+2);
   strcpy(*result, str);
   MLReleaseString(lp, str);
}


//F = MINF(conf, F1, F2) <=> [x^n]F = MIN([x^n]F1, [x^n]F2)
void MINF(CONFIGURE *conf, MLINK lp, char* ex1, char* ex2, char** result){   
   if (strcmp(ex1, "0") || strcmp(ex2, "0")){
      char* query = malloc(strlen(ex1) + 50); 
      if (DEBUG)
         printf("Expression %s \n %s\n", ex1, ex2);

      //send F1 to Mathematica
      sprintf(query, "e[X_] = Normal[Series[%s,  {X, 0, %d}]];", ex1, conf->max_gf);

      MLPutFunction(lp, "EnterTextPacket", 1);
      MLPutString(lp, query);
      MLEndPacket(lp);

      //send F2 to Mathematica
      char* query2 = malloc(strlen(ex2) + 50); 
      sprintf(query2, "f[X_] = Normal[Series[%s,  {X, 0, %d}]];", ex2, conf->max_gf);

      MLPutFunction(lp, "EnterTextPacket", 1);
      MLPutString(lp, query2);
      MLEndPacket(lp);

      //compute F
      char query3[200] = "With[{me = Max @ Exponent[{e[X], f[X]}, X]}, X^Range[0, me].(Min@Coefficient[{e[X], f[X]}, X, #] & /@ Range[0, me])]";
      MLPutFunction(lp, "EnterTextPacket", 1);
      MLPutString(lp, query3);
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
         *result = malloc(strlen(str)+2);
         strcpy(*result, str);
         MLReleaseString(lp, str);
      }
   } else{
      *result = malloc(3);
      strcpy(*result, "0");
   }
}

//F = MAXF(conf, F1, F2) <=> [x^n]F = MAX([x^n]F1, [x^n]F2)
void MAXF(CONFIGURE *conf, MLINK lp, char* ex1, char* ex2, char** result){   
   if (strcmp(ex1, "0") || strcmp(ex2, "0")){
      char* query = malloc(strlen(ex1) + 50);
      //send F1 to Mathematica             
      sprintf(query, "e[X_] = Normal[Series[%s,  {X, 0, %d}]];", ex1, conf->max_gf);

      MLPutFunction(lp, "EnterTextPacket", 1);
      MLPutString(lp, query);
      MLEndPacket(lp);

      //send F2 to Mathematica
      char* query2 = malloc(strlen(ex2) + 50); 
      sprintf(query2, "f[X_] = Normal[Series[%s,  {X, 0, %d}]];", ex2, conf->max_gf);

      MLPutFunction(lp, "EnterTextPacket", 1);
      MLPutString(lp, query2);
      MLEndPacket(lp);

      //compute MAXF
      if (strcmp(ex1, "0") == 0){
         char query3[200] = "With[{me = Exponent[f[X], X]}, X^Range[0, me].(Max@Coefficient[0, f[X]}, X, #] & /@ Range[0, me])]";
         MLPutFunction(lp, "EnterTextPacket", 1);
         MLPutString(lp, query3);
         MLEndPacket(lp);
      } else 
      if (strcmp(ex2, "0") == 0)
      {
         char query3[200] = "With[{me = Exponent[e[X], X]}, X^Range[0, me].(Max@Coefficient[{e[X], 0}, X, #] & /@ Range[0, me])]";
         MLPutFunction(lp, "EnterTextPacket", 1);
         MLPutString(lp, query3);
         MLEndPacket(lp);
      } else
      {   
         char query3[200] = "With[{me = Max @ Exponent[{e[X], f[X]}, X]}, X^Range[0, me].(Max@Coefficient[{e[X], f[X]}, X, #] & /@ Range[0, me])]";
         MLPutFunction(lp, "EnterTextPacket", 1);
         MLPutString(lp, query3);
         MLEndPacket(lp);
      }


      MLPutFunction(lp, "EvaluatePacket", 1);
      MLPutFunction(lp, "ToExpression", 1);
      MLPutString(lp, "ToString[%, InputForm]");
      MLEndPacket(lp);

      int pkt;
      /* skip any packets before the first ReturnPacket */
      while ( (pkt = MLNextPacket( lp), pkt) && pkt != RETURNPKT)
      {
         MLNewPacket(lp);
         if ( MLError(lp)) error(lp);
      }

      const char* str;
      if(! MLGetString(lp, &str))        
            printf("%s\n", "Unable to read string from Mathamatica");
      else{         
         *result = malloc(strlen(str)+2);
         strcpy(*result, str);         
         MLReleaseString(lp, str);
         // printf("%s\n", result);
      }
      free(query);
      free(query2);
   } else{
      *result = malloc(10);
      strcpy(*result, "0");         
   }
}

//calculate several operators between two GFs
void calc(CONFIGURE *conf, MLINK lp, char* ex1, char* ex2, char** result, char opr){
   char* query = malloc(strlen(ex1) + 50); 
   sprintf(query, "e[X_] = %s;", ex1);

   MLPutFunction(lp, "EnterTextPacket", 1);
   MLPutString(lp, query);
   MLEndPacket(lp);

   char* query2 = malloc(strlen(ex2) + 50); 
   sprintf(query2, "f[X_] = %s;", ex2);

   MLPutFunction(lp, "EnterTextPacket", 1);
   MLPutString(lp, query2);
   MLEndPacket(lp);

   char* query3 = malloc (50);
   sprintf(query3, "e[X] %c f[X]", opr);
   MLPutFunction(lp, "EnterTextPacket", 1);
   MLPutString(lp, query3);
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
      *result = malloc(strlen(str) + 1);
      strcpy(*result, str);
      MLReleaseString(lp, str);
   }

   char* gz = malloc(15);      
   sprintf(gz, "1/(1-%dX)", conf->alphabet_size);
   //Keep the result smaller than Gz
   if (opr == '+' || opr == '*')
      MINF(conf, lp, *result, gz, result);

   //and greater than 0
   //for QIFS, we only use - when we change the length in the final step
   //the result will always >= 0 then no need this step
   // if (strcmp(*result, "0") && opr == '-')
   //    MAXF(conf, lp, *result, "0", result);
}



//calculate several operators between two GFs, allow negative values or 
//values that go over G
void calc_special(CONFIGURE *conf, MLINK lp, char* ex1, char* ex2, char** result, char opr){
   char* query = malloc(strlen(ex1) + 50); 
   sprintf(query, "e[X_] = %s;", ex1);

   MLPutFunction(lp, "EnterTextPacket", 1);
   MLPutString(lp, query);
   MLEndPacket(lp);

   char* query2 = malloc(strlen(ex2) + 50); 
   sprintf(query2, "f[X_] = %s;", ex2);

   MLPutFunction(lp, "EnterTextPacket", 1);
   MLPutString(lp, query2);
   MLEndPacket(lp);

   char* query3 = malloc (50);
   sprintf(query3, "e[X] %c f[X]", opr);
   MLPutFunction(lp, "EnterTextPacket", 1);
   MLPutString(lp, query3);
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
      *result = malloc(strlen(str) + 1);
      strcpy(*result, str);
      MLReleaseString(lp, str);
   }
}


//As definition in the paper.
void SUBF(CONFIGURE *conf, MLINK lp, char* poly, int number, char** result){
   if (number < 0 || !strcmp(poly, "0")){
      *result = malloc(2);
      sprintf(*result, "0");
      return;
   }

   if (number > conf->max_gf)
      number = conf->max_gf;

   char* query = malloc(strlen(poly) + 100); 
   sprintf(query, "e[X_] = Normal[Series[%s,  {X, 0, %d}]]", poly, number);
   if (DEBUG)
      printf("Expression %s\n", poly);

   MLPutFunction(lp, "EnterTextPacket", 1);
   MLPutString(lp, query);
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
      if (DEBUG)
         printf("%s \n", str);
      *result = malloc(strlen(str)+2);
      strcpy(*result, str);
      MLReleaseString(lp, str);
   }

   free(query);   
}

/*
F1(P) set a_i = 1 if a_i > 0
 */
void F1(CONFIGURE *conf, MLINK lp, char* P, char** result){
   MINF(conf, lp, P, "1/(1-X)", result);
}

//return min and max exponent of polynomial p1
void get_min_max_exponent(CONFIGURE *conf, MLINK lp, char* p1, int* minE, int* maxE){
   char* query = malloc(strlen(p1) + 100); 
   sprintf(query, "a = Normal[Series[%s,  {X, 0, %d}]];", p1, conf->max_gf);

   MLPutFunction(lp, "EnterTextPacket", 1);
   MLPutString(lp, query);
   MLEndPacket(lp);


   MLPutFunction(lp, "EvaluatePacket", 1);
   MLPutFunction(lp, "ToExpression", 1);
   MLPutString(lp, "Exponent[a, X]");
   MLEndPacket(lp);

   int pkt;
   while ( (pkt = MLNextPacket( lp), pkt) && pkt != RETURNPKT)
   {
      MLNewPacket(lp);
      if ( MLError(lp)) error( lp);
   }

   int exponent1;   
   if(! MLGetInteger(lp, &exponent1))        
         printf("%s\n", "Unable to read Integer from Mathamatica");
   *maxE = exponent1;

   //get min exponent
   char* query2 = malloc(100);
   sprintf(query2, "Cases[Reverse@CoefficientRules[a],Rule[c_,b_?Positive]->(c),1,1]");
   MLPutFunction(lp, "EnterTextPacket", 1);
   MLPutString(lp, query2);
   MLEndPacket(lp);

   MLPutFunction(lp, "EvaluatePacket", 1);
   MLPutFunction(lp, "ToExpression", 1);
   MLPutString(lp, "ToString[%, InputForm]");
   MLEndPacket(lp);

   while ((pkt = MLNextPacket( lp), pkt) && pkt != RETURNPKT)
   {
      MLNewPacket(lp);
      if ( MLError(lp)) error( lp);
   }

   const char* minExponent;
   if(! MLGetString(lp, &minExponent))        
         printf("%s\n", "Unable to read String from Mathamatica");
   
   char* tmp = malloc(strlen(minExponent));
   //result will have form of {{Number}}
   strncpy(tmp, minExponent+2, strlen(minExponent)-4);
   tmp[strlen(minExponent)-4] = '\0';
   *minE = atoi(tmp);
   MLReleaseString(lp, minExponent);

   free(query);
}

/*
Get lower bound for res = p1.p2 in precise version.
Read paper for more details
 */
void get_lower(CONFIGURE *conf, MLINK lp, char* p1, char* p2, int minE, int maxE, char** res){
   //if either p1 or p2 equals to 0, they returns 0
   if (DEBUG)
      printf("Get lower...\n");   

   char* tmp = malloc(10);
   sprintf(tmp, "0");
   int hit = 0;
   int runConcat = 0, froml = 0;
   int lexponent = minE, uexponent = maxE;

   while (hit < conf->concat_iters && lexponent <= uexponent){
      char* coef;
      int exponent = froml?lexponent:uexponent;
      if (DEBUG)
         printf("Concat %d(th) try with exponent %d...\n", ++runConcat, exponent);
      
      get_coef(conf, lp, p1, exponent, &coef);
      if (strcmp("0", coef)){        
         char* mul;
         char* operand = malloc(strlen(coef) + 20);
         
         sprintf(operand, "%s*X^%d", coef, exponent);
         calc(conf, lp, operand, p2, &mul, '*');
         

         if (strcmp(mul, "0"))
            if (hit++)
                  MAXF(conf, lp, tmp, mul, &tmp);      
            else{
               tmp = malloc(strlen(mul) + 1);
               strcpy(tmp, mul);               
            }
         free(mul);
         free(operand);
      }
      free(coef);
      if (froml) lexponent++;
      else uexponent--;
      froml = 1-froml;      
   }
   
   *res = malloc(strlen(tmp) + 1);
   strcpy(*res, tmp);    
   free(tmp);
}

/*
Get lower bound for res = p1.p2 in precise version.
Read paper for more details
 */
void optimize_concat(CONFIGURE *conf, MLINK lp, char* p1, char* p2, char** res){
   if (!(strcmp(p1, "0") && strcmp(p2, "0"))){
      *res = malloc(10);
      sprintf(*res, "0");
      return;
   }

   int minE1, maxE1;
   get_min_max_exponent(conf, lp, p1, &minE1, &maxE1);
   if (DEBUG)
      printf("Range of exponent of P1 %d %d\n", minE1, maxE1);

   int minE2, maxE2;
   get_min_max_exponent(conf, lp, p2, &minE2, &maxE2);
   if (DEBUG)
      printf("Range of exponent of P2 %d %d\n", minE2, maxE2);
   //
   if (maxE1 - minE1 <= conf->concat_iters){
      get_lower(conf, lp, p1, p2, minE1, maxE1, res);
      return;
   }

   //
   if (maxE2 - minE2 <= conf->concat_iters){
      get_lower(conf, lp, p2, p1, minE2, maxE2, res);
      return;
   }


   char* tmp;
   get_lower(conf, lp, p1, p2, minE1, maxE1, res);
   get_lower(conf, lp, p2, p1, minE2, maxE2, &tmp);
   MAXF(conf, lp, tmp, *res, res);
}


/*
get coefficient a_number of polynomial
 */
void get_coef(CONFIGURE *conf, MLINK lp, char* poly, int number, char** result){
   if (number < 0){
      printf("Out of range \n");
      return;
   }   

   if (number > conf->max_gf)
      number = conf->max_gf;

   char* query = malloc(strlen(poly) + 150); 
   sprintf(query, "Coefficient[Normal[Series[%s,  {X, 0, %d}]], X, %d]", poly, number, number);
   if (DEBUG)
      printf("Geting Coefficient ====>>>> Expression %s\n", poly);

   MLPutFunction(lp, "EnterTextPacket", 1);
   MLPutString(lp, query);
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
      *result = malloc(strlen(str)+2);
      strcpy(*result, str);
   }
   MLReleaseString(lp, str);
   free(query);   
   if (DEBUG)
      printf("Coefficient result %s \n", *result);
}

char* NOT(CONFIGURE *conf, MLINK lp, bounds input, bounds* results)
{
   char* expression = malloc(strlen(input.upperBound) + 30);
   sprintf(expression, "1/(1-%dX) - (%s)", conf->alphabet_size, input.upperBound);
   
   char* sresult;
   send_to_mathematica(lp, expression, &sresult);

   (*results).lowerBound = malloc(strlen(sresult) + 1);
   strcpy((*results).lowerBound, sresult);

   char* ex2 = malloc(strlen(input.lowerBound) + 30);
   sprintf(ex2, "1/(1-%dX) - (%s)", conf->alphabet_size, input.lowerBound);
   send_to_mathematica(lp, expression, &sresult);

   (*results).upperBound = malloc(strlen(sresult) + 1);
   strcpy((*results).upperBound, sresult);

   free(sresult);
   free(expression);
   free(ex2);
}

/*
Compute conjunction.
L = 0
U = MINF(conf, U1, U2)
 */
void AND(CONFIGURE *conf, MLINK lp, bounds input1, bounds input2, bounds* results){
   char* str;
   MINF(conf, lp, input1.upperBound, input2.upperBound, &str);
   
   if (DEBUG)
      printf("%s\n %s\n", input1.lowerBound, input2.lowerBound);
   (*results).upperBound = malloc(strlen(str) + 1);
   strcpy((*results).upperBound, str);

   calc_special(conf, lp, input1.lowerBound, input2.lowerBound, &str, '+');

   char* G = malloc(100);
   char* sub;
   sprintf(G, "1/(1-%dX)", conf->alphabet_size);
   calc_special(conf, lp, str, G, &sub, '-');
   // printf("%s\n", sub);
   MAXF(conf, lp, sub, "0", &str);

   (*results).lowerBound = malloc(strlen(str) + 1);
   strcpy((*results).lowerBound, str);

   free(sub);
   free(str);
}

/*
Compute disjunction.
L = MAXF(conf, L1, L2)
U = MINF(conf, U1 + U2, G)
 */
void OR(CONFIGURE *conf, MLINK lp, bounds input1, bounds input2, bounds* results){
   char* str; 
   MAXF(conf, lp, input1.lowerBound, input2.lowerBound, &str);

   (*results).lowerBound = malloc(strlen(str)+1);
   strcpy((*results).lowerBound, str);

   calc(conf, lp, input1.upperBound, input2.upperBound, &str, '+');
   (*results).upperBound =malloc(strlen(str)+1);
   strcpy((*results).upperBound, str);

   free(str);
}

/*
Copy bounds
 */
void copy_bounds(bounds input, bounds* results){
   (*results).upperBound =malloc(strlen(input.upperBound)+1);
   strcpy((*results).upperBound, input.upperBound);

   (*results).lowerBound =malloc(strlen(input.lowerBound)+1);
   strcpy((*results).lowerBound, input.lowerBound);
}
/*
Copy bounds from concrete strings
 */
void copy_bounds_concrete(char* lower, char* upper, bounds* results){

   (*results).lowerBound = malloc(strlen(lower)+1);
   strcpy((*results).lowerBound, lower);

   (*results).upperBound =malloc(strlen(upper)+1);
   strcpy((*results).upperBound, upper);
}
/*
switch length operators for not_case
 */
void switch_operator(char** op){
   if (strstr(*op, ">="))
      *op = "";
   else if (strstr(*op, "<="))
      *op = ">";
   else if (strstr(*op, "<"))
      *op = ">=";
   else if (strstr(*op, ">"))
      *op = "<=";
   else if (strstr(*op, "="))
      *op = "!=";
   else if (strstr(*op, "="))
      *op = "!=";
}
/**
 * get the max value of two numbers
 * @param  x  first number
 * @param  y second number
 * @return   max value
 */
int max(int x, int y){
   return x < y?y:x;
}


/**
 * get the min value of two numbers
 * @param  x  first number
 * @param  y second number
 * @return   min value
 */
int min(int x, int y){
   return x < y?x:y;
}


/**
 * get absolute value of number x
 * @param  x [the number]
 * @return   [the absoulute value of x]
 */
double get_abs(double x){
   return x < 0? -x: x;
}

/*
This function builds the auto correlation table of string s
and stores the result into the table ctable
*/
void compute_correlation_table(char* s, int ctable[], int size){
   int i, j;
   for (i=0; i<size; i++)
   {
      ctable[i]=1;
      for (j = i;  j<size; j++) if (s[j]!=s[j-i]){
         ctable[i] = 0;
         break;
      }
   }
}


/*
This function builds the reverse correlation table of string s
and stores the result into the table reverse_ctable
*/
void compute_reverse_correlation_table(char* s, int reverse_ctable[], int size){
   int i, j;
   for (i=size-1; i>=0; i--)
   {
      reverse_ctable[i]=1;
      for (j = 0; j<=i; j++) if (s[j]!=s[j+size-i-1]){
         reverse_ctable[i] = 0;
         break;
      }
   }
}


/*
Compute S != S1 && S != S2 && ... && S != Sn
 */
void compute_n_not_equal(CONFIGURE *conf, MLINK lp, char** s, int n, bounds* result){
   int i;
   int len[MAX_PATTERN_LEN+1];

   for (i = 0; i <= MAX_PATTERN_LEN; i++)
      len[i] = 0;
   for (i = 0; i < n; i++)
      len[strlen(s[i])]++;
   int res_len = 20;
   //
   for (i = 0; i <= MAX_PATTERN_LEN; i++)
      if (len[i] > 0)
         // +len[i]*X^i
         res_len += 15;
   char* res = malloc(res_len);
   sprintf(res, "1/(1-%dX)-(0", conf->alphabet_size);
   
   for (i = 0; i < MAX_PATTERN_LEN; i++)
      if (len[i] > 0)
         // +len[i]*X^i
      {
         char* tmp = malloc(20);
         sprintf(tmp, "+%d*X^%d", len[i], i);
         strcat(res, tmp);
      }
   strcat(res, ")");
   char* final;
   send_to_mathematica(lp, res, &final);
   copy_bounds_concrete(final, final, result);
   free(final);
   free(res);
}

/*
Compute S = S1 || S = S2 || ... || S = Sn
 */
void compute_or_n_equal(CONFIGURE *conf, MLINK lp, char** s, int n, bounds* result){
   int i;
   int len[MAX_PATTERN_LEN+1];   

   for (i = 0; i <= MAX_PATTERN_LEN; i++)
      len[i] = 0;
   for (i = 0; i < n; i++)
      len[strlen(s[i])]++;
   //compute the length of the result GF
   //start with 20
   int res_len = 20;
   for (i = 0; i <= MAX_PATTERN_LEN; i++)
      if (len[i] > 0)
         // +len[i]*X^i
         res_len += 15;
   char* res = malloc(res_len);
   sprintf(res, "(0");

   //build the GF res
   for (i = 0; i < MAX_PATTERN_LEN; i++)
      if (len[i] > 0)
         // +len[i]*X^i
      {
         char* tmp = malloc(20);
         sprintf(tmp, "+%d*X^%d", len[i], i);
         strcat(res, tmp);
      }
   strcat(res, ")");
   char* final;
   send_to_mathematica(lp, res, &final);
   // printf("final %s\n", final);
   copy_bounds_concrete(final, final, result);

   free(final);
   free(res);
}


// void get_min_max_from_mathematica(MLINK lp, char* query, int** min_len, int** max_len);

void preprocess_or(CONFIGURE *conf, MLINK lp, FILE *file, bounds vars[], int i);
void update_length(CONFIGURE *conf, MLINK lp, bounds vars[], int nVar);

/*
The aim of preprocess are to get the bounds of length for all variables 
and decide which variable should have the lower bound of 0
 */
void preprocess(CONFIGURE *conf, MLINK lp, FILE *file, bounds vars[], int nVar)
{      
   int code;
   int i = -1;
   int not_case = 0;
   while (fscanf(file, "%d", &code) != EOF) {
      if (code == CODE_NEW_VAR){               
         i++;
         initialize(conf, &vars[i], nVar);
         fscanf(file, "%s", vars[i].name);         
         if (DEBUG)
            printf("Preprocess variable %s\n", vars[i].name);
      }

      //contains
      if (code == CODE_CONTAINS){
         char* pattern =  malloc (MAX_PATTERN_LEN);
         fscanf(file, "%s", pattern);
         free(pattern);
         vars[i].content_opr = 1;
      }

      //strstr
      if (code == CODE_STRSTR){
         char* pattern = malloc (MAX_PATTERN_LEN);
         int position;
         fscanf(file, "%s %d", pattern, &position);
         free(pattern);
         vars[i].content_opr = 1;
      }

      //equal, 
      // for a = b, just pass since we already merge their constraints.
      //for ! a = b, get GF of b and handle as normal
      if (code == CODE_EQUAL){
         char* var1 =  malloc (MAX_PATTERN_LEN);
         fscanf(file, "%s", var1);
         vars[i].content_opr = 1;
      }

      //concat
      if (code == CODE_CONCAT){
         // printf("Concat\n");
         char* var1 =  malloc (MAX_PATTERN_LEN);
         char* var2 =  malloc (MAX_PATTERN_LEN);
         fscanf(file, "%s", var1);
         fscanf(file, "%s", var2);
         int j, p1 = -1, p2 = -1;
         for (j = 0; j < i; j++){
            if (strcmp(vars[j].name, var1) == 0)
               p1 = j;
            if (strcmp(vars[j].name, var2) == 0)
               p2 = j;
         }

         if (p1 >= 0 && p2 >= 0 && !not_case){
            vars[i].nconcat += insert_int(vars[i].concat_list, p1);
            vars[i].nconcat += insert_int(vars[i].concat_list, p2);
            //if there are more than two concat operators
            if (vars[i].nconcat > 2)
               vars[i].content_opr = 1;            
         }
         // printf("Done concat\n");
      }

      //length
      if (code == CODE_LENGTH){
         char* op =  malloc (3);
         fscanf(file, "%s", op);
         int number;
         fscanf(file, "%d", &number);
         
         if (not_case)
            switch_operator(&op);
         if (strstr(op, ">="))
            vars[i].lenL = max(vars[i].lenL, number);
         else if (strstr(op, "<="))
            vars[i].lenU = min(vars[i].lenU, number);
         else if (strstr(op, "<"))
            vars[i].lenU = min(vars[i].lenU, number-1);
         else if (strstr(op, ">"))
            vars[i].lenL = max(vars[i].lenL, number+1);
         else if (strstr(op, "!=")){            
            if (number == vars[i].lenL)
               vars[i].lenL++;
            else if (number == vars[i].lenU)
               vars[i].lenU--;
            else
               //@TODO build a list of not equal lengths
               vars[i].content_opr = 1;
         }
         else if (strstr(op, "=")){
            vars[i].lenL = max(vars[i].lenL, number);
            vars[i].lenU = min(vars[i].lenU, number);
         }
      }

      //match regex
      if (code == CODE_REGEX){
         char* pattern =  malloc(MAX_REGEX_LEN);
         fscanf(file, "%s", pattern);
         vars[i].content_opr = 1;
         free(pattern);
      }

      //with single string
      if (code == CODE_EQUAL_CSTR){         
         char* pattern = malloc(MAX_PATTERN_LEN);
         fscanf(file, "%s", pattern);
         vars[i].content_opr = 1;
         if (!not_case){
            vars[i].lenL = max(vars[i].lenL, strlen(pattern));
            vars[i].lenU = min(vars[i].lenU, strlen(pattern));
         }
         free(pattern);         
      }      

      //not case
      if (code == CODE_NOT) not_case = 1;
      else not_case = 0;

      //list of or, doing the same thing again
      if (code == CODE_OR){         
         preprocess_or(conf, lp, file, vars, i);
         vars[i].content_opr = 1;         
      }

      if (code == CODE_QUERY){
         int len;
         fscanf(file, "%d", &len);         
      }
   }

   update_length(conf, lp, vars, nVar);
}


/*
read the or combination
 */
void preprocess_or(CONFIGURE *conf, MLINK lp, FILE *file, bounds vars[], int i)
{
   int code;
   int not_case = 0;
   int iE, nExp, upper;
   fscanf(file, "%d %d", &nExp, &upper);
   for (iE = 0; iE < nExp; iE++){
      fscanf(file, "%d", &code);

      //contains
      if (code == CODE_CONTAINS){
         char* pattern =  malloc (MAX_PATTERN_LEN);
         fscanf(file, "%s", pattern);
         free(pattern);
      }

      //strstr
      if (code == CODE_STRSTR){
         char* pattern = malloc (MAX_PATTERN_LEN);
         int position;
         fscanf(file, "%s %d", pattern, &position);
         free(pattern);  
      }

      //equal, 
      if (code == CODE_EQUAL){
         char* var1 =  malloc (MAX_PATTERN_LEN);
         fscanf(file, "%s", var1);            
      }

      //concat
      if (code == CODE_CONCAT){
         char* var1 =  malloc (MAX_PATTERN_LEN);
         char* var2 =  malloc (MAX_PATTERN_LEN);
         fscanf(file, "%s", var1);
         fscanf(file, "%s", var2);
      }

      //length
      if (code == CODE_LENGTH){
         char* op =  malloc (3);
         fscanf(file, "%s", op);
         int number;
         fscanf(file, "%d", &number);

         if (not_case)
            switch_operator(&op);
      }

      //match regex
      if (code == CODE_REGEX){
         char* pattern =  malloc(MAX_REGEX_LEN);
         fscanf(file, "%s", pattern);
         free(pattern);
      }

      //with single string
      if (code == CODE_EQUAL_CSTR){
         char* pattern = malloc(MAX_PATTERN_LEN);
         fscanf(file, "%s", pattern);
         free(pattern);
      }

      //not case
      if (code == CODE_NOT) not_case = 1;
      else not_case = 0;
   }   
}


//update the range of length of each variables
void update_length(CONFIGURE *conf, MLINK lp, bounds vars[], int nVar){
   if (DEBUG)
      printf("Start updating length of variables\n");
   int i;   
   for (i=0; i < nVar; i++){
      if (vars[i].nconcat > 0){
         int temp[vars[i].nconcat];
         from_int_list_to_array(vars[i].concat_list, temp);
         int j=0;
         while (j < vars[i].nconcat){
            int p1 = temp[j++];
            int p2 = temp[j++];
            //change the length
            vars[i].lenU = min(vars[i].lenU, vars[p1].lenU + vars[p2].lenU);
            vars[i].lenL = max(vars[i].lenL, vars[p1].lenL + vars[p2].lenL);

            vars[p1].lenU = min(vars[p1].lenU, vars[i].lenU - vars[p2].lenL);
            vars[p2].lenU = min(vars[p2].lenU, vars[i].lenU - vars[p1].lenL);

            vars[p1].lenL = max(vars[p1].lenL, vars[i].lenL - vars[p2].lenU);
            vars[p2].lenL = max(vars[p2].lenL, vars[i].lenL - vars[p1].lenU);
            //if a = b.c and a has content operator(s) then we set lower bound
            //of a, b, c as zero
            if (vars[i].content_opr){
               vars[p1].zero_lower_bound = 1;               
               vars[p2].zero_lower_bound = 1;               
               vars[i].zero_lower_bound = 1;               
            }
         } //end while
      } //end if
   } //end for            
}


void make_simple_call(MLINK lp){
   
   MLPutFunction(lp, "EnterTextPacket", 1);
   MLPutString(lp, "1+1");
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
   MLReleaseString(lp, str);   
}

void read_configuration(CONFIGURE *conf){
   FILE* file = fopen("smc.conf", "r");
   if (file == NULL) {
      fprintf(stderr, "smc.conf file not found. Execution stops.", errno);
      return;
   }
   conf->max_gf = 0;
   conf->alphabet_size = 0;
   conf->opt_concat_optimize = 0;
   conf->concat_iters = 0;

   char line[1000];   
   while (fgets(line, sizeof(line), file)) {
         char *pch;         
         if (pch = strstr(line, "MAX_GF"))
            conf->max_gf = atoi(pch+6);
         else if (pch = strstr(line, "ALPHABET_SIZE"))
            conf->alphabet_size = atoi(pch+13);
         else if (pch = strstr(line, "OPT_CONCAT_OPTIMIZE"))
            conf->opt_concat_optimize = atoi(pch+19);
         else if (pch = strstr(line, "CONCAT_ITERS"))
            conf->concat_iters = atoi(pch+12);
         else
            continue;
    }
   
   fclose(file);
}

void print_configuration(CONFIGURE *conf){
   printf("MAX_GF: %d\n", conf->max_gf);
   printf("ALPHABET_SIZE: %d\n", conf->alphabet_size);
   printf("OPT_CONCAT_OPTIMIZE: %d\n", conf->opt_concat_optimize);
   printf("CONCAT_ITERS: %d\n", conf->concat_iters);
}