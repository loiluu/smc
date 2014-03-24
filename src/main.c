//export PATH=/usr/bin:$PATH
#include "utils.h"
#include "contains.h"
#include "match_regex.h"
#include <string.h>
#include <stdio.h>
#include <sys/time.h>
#include "list.h"
#include "mathlink.h"
#include <stdlib.h>
#include "config.h"
#define DEBUG 0


void complete_compute(CONFIGURE *conf, MLINK lp, bounds* input);
void complete_compute_OR(CONFIGURE *conf, MLINK lp, bounds* input, int upper);
void computeOR(CONFIGURE *conf, MLINK lp, FILE *file, bounds* return_results, bounds vars[], int i);

int main(int argc, char *argv[]){
   //get configuration from smc.conf
   CONFIGURE *conf = malloc(sizeof(CONFIGURE));
   read_configuration(conf);
   print_configuration(conf);
   
   //open connection
   MLINK lp;
   int pkt;
   MLEnvironment env;
   env = MLInitialize(NULL);
   if (env == NULL) return;
   int argcs = 4;
   char *argvs[5] = {"-linkmode", "launch", "-linkname", "math -mathlink", NULL};
   lp = MLOpen(argcs, argvs);
   if (lp == NULL) return;
      
   
   //start timing   
   struct timeval start, end;
   int rc;
   rc=gettimeofday(&start, NULL);

   //This is a dummy call to Mathematica to make sure that
   //we call it at least once
   //it only requires in my machine, please check if it happens in yours
   //if not, just remove the following line
   make_simple_call(lp);

   char input_file[100] = "input.txt";
   int nVar;
   if (argc == 2)
      strcpy(input_file, argv[1]);

   if (DEBUG)
      printf("Start preprocessing\n");
   
   FILE *file;
   //get the length bounds
   file = fopen(input_file, "r");  
   fscanf(file, "%d", &nVar);
   bounds vars[nVar];
   preprocess(conf, lp, file, vars, nVar);      
   fclose(file);   
   if (DEBUG)
      printf("Done preprocessing\n");

   //open the file again
   file = fopen(input_file, "r"); 
   fscanf(file, "%d", &nVar);

   int code;
   int query_var;
   int query_len;
   int i = -1;
   int not_case = 0;
   while (fscanf(file, "%d", &code) != EOF) {
      if (code == CODE_NEW_VAR){
         if (i >= 0)
            complete_compute(conf, lp, &vars[i]);         
         i++;         
         fscanf(file, "%s", vars[i].name);
         if (DEBUG)
            printf("Working with %s ...\n", vars[i].name);         
      }

      //contains
      if (code == CODE_CONTAINS){
         char* pattern =  malloc (MAX_PATTERN_LEN);
         fscanf(file, "%s", pattern);
         if (DEBUG) printf("%d %s\n", code, pattern);
         if (not_case)
            vars[i].nc += insert(vars[i].not_contains, pattern);
         else{
            if (DEBUG)
               printf("Computing constains %s\n", pattern);
            if (vars[i].lowerBound == NULL)
               count_contains(conf, lp, pattern, &vars[i]);
            else{
               bounds tmp;
               count_contains(conf, lp, pattern, &tmp);
               AND(conf, lp, vars[i], tmp, &vars[i]);
            }
         }
         vars[i].content_opr = 1;
      }

      //strstr
      if (code == CODE_STRSTR){
         char* pattern = malloc (MAX_PATTERN_LEN);
         int position;
         fscanf(file, "%s %d", pattern, &position);
         if (DEBUG) 
            printf("%d %s %d\n", code, pattern, position);

         bounds tmp;
         count_strstr(conf, lp, pattern, position, &tmp);         
         if (not_case)
            NOT(conf, lp, tmp, &tmp);

         if (vars[i].lowerBound == NULL)
            copy_bounds(tmp, &vars[i]);
         else
            AND(conf, lp, vars[i], tmp, &vars[i]);
         vars[i].content_opr = 1;
      }

      //equal, 
      //for a = b, just pass since we already merge their constraints.
      //for ! a = b, get GF of b and handle as normal
      if (code == CODE_EQUAL){
         char* var1 =  malloc (MAX_PATTERN_LEN);
         fscanf(file, "%s", var1);
         if (DEBUG)
            printf("%d %s\n", code, var1);
         if (not_case){
            int j, p1 = -1;
            for (j = 0; j < i; j++)
               if (strcmp(vars[j].name, var1) == 0)
            if (j > 0){
               bounds tmp;
               copy_bounds(vars[j], &tmp);
               NOT(conf, lp, tmp, &tmp); 
               AND(conf, lp, vars[i], tmp, &vars[i]);
            }
         }    
      }

      //concat
      if (code == CODE_CONCAT){
         char* var1 =  malloc (MAX_PATTERN_LEN);
         char* var2 =  malloc (MAX_PATTERN_LEN);
         fscanf(file, "%s", var1);
         fscanf(file, "%s", var2);
         if (DEBUG)
            printf("%d %s %s\n", code, var1, var2);
         int j, p1 = -1, p2 = -1;
         for (j = 0; j < i; j++){
            if (strcmp(vars[j].name, var1) == 0)
               p1 = j;
            if (strcmp(vars[j].name, var2) == 0)
               p2 = j;
         }

         if (p1 >= 0 && p2 >= 0){            
            char* tmp;
            char* tmp1;            
                        
            calc(conf, lp, vars[p1].upperBound, vars[p2].upperBound, &tmp1, '*');
            
            if (conf->opt_concat_optimize){
               optimize_concat(conf, lp, vars[p1].lowerBound, vars[p2].lowerBound, &tmp);
            }
            else{
               calc(conf, lp, vars[p1].lowerBound, vars[p2].lowerBound, &tmp, '*');
               F1(conf, lp, tmp, &tmp);
            }
            
            bounds data;
            copy_bounds_concrete(tmp, tmp1, &data);
            if (not_case)
               NOT(conf, lp, data, &data);               
            
            if (vars[i].lowerBound == NULL)
               copy_bounds(data, &vars[i]);
            else
               AND(conf, lp, vars[i], data, &vars[i]);
            
            free(tmp);
            free(tmp1);            
         }
      }

      //length
      if (code == CODE_LENGTH){
         char* op =  malloc (2);
         fscanf(file, "%s", op);
         int number;
         fscanf(file, "%d", &number);
         if (DEBUG)
            printf("%d %s %d\n", code, op, number);                 
      }

      //match regex
      if (code == CODE_REGEX){
         char* pattern =  malloc(MAX_REGEX_LEN);
         fscanf(file, "%s", pattern);
         if (DEBUG)
            printf("Match regex %d %s\n", code, pattern);

         if (not_case){
            vars[i].nm += insert(vars[i].not_match, pattern);
         }
         else{
            vars[i].n_and_match += insert(vars[i].and_match, pattern);
            // if (vars[i].lowerBound == NULL)
            //    count_match_regex(conf, lp, pattern, &vars[i]);
            // else{
            //    bounds tmp;
            //    count_match_regex(conf, lp, pattern, &tmp);
            //    AND(conf, lp, vars[i], tmp, &vars[i]);
            // }
         }
         vars[i].content_opr = 1;
      }

      //with single string
      if (code == CODE_EQUAL_CSTR){
         char* pattern = malloc(MAX_PATTERN_LEN);
         fscanf(file, "%s", pattern);
         if (DEBUG)
            printf("Compare to %s\n", pattern);

         //store the list of constStrings that var != constStr
         if (not_case){
            vars[i].ne += insert(vars[i].not_equal, pattern);
         }
         else{
            // = constString
            char* tmp = malloc(15);
            sprintf(tmp, "X^%d", (int) strlen(pattern));
            bounds btmp;
            copy_bounds_concrete(tmp, tmp, &btmp);
            if (vars[i].lowerBound == NULL)
               copy_bounds(btmp, &vars[i]);
            else
               AND(conf, lp, vars[i], btmp, &vars[i]);            
         }
      }

      //is AllString
      if (code == CODE_ALL_STR){
         char* gz = malloc(15);      
         sprintf(gz, "1/(1-%dX)", conf->alphabet_size);      
         copy_bounds_concrete(gz, gz, &vars[i]);         
      }

      //not case
      if (code == CODE_NOT) not_case = 1;
      else not_case = 0;

      //list of or, doing the same thing again
      if (code == CODE_OR){
         bounds result;
         computeOR(conf, lp, file, &result, vars, i);
         if (vars[i].lowerBound == NULL)
            copy_bounds(result, &vars[i]);
         else
            AND(conf, lp, vars[i], result, &vars[i]);
         vars[i].content_opr = result.content_opr;
      }

      if (code == CODE_QUERY){
         query_var = i;
         fscanf(file, "%d", &query_len);
      }
   }
      
   complete_compute(conf, lp, &vars[nVar-1]);
   // complete_concat(lp, vars, nVar);
   
   //printf results
   FILE *f = fopen("output.txt", "w");
   if (f == NULL)
   {
       printf("Error opening output file!\n");
       exit(1);
   }
   
   if (query_len){
      if (DEBUG)
         printf("Printing results to file...\n");
      char* lower_result;
      char* upper_result;
      get_coef(conf, lp, vars[query_var].lowerBound, query_len, &lower_result);
      get_coef(conf, lp, vars[query_var].upperBound, query_len, &upper_result);
      fprintf(f, "%s\n lowerBound: %s \n upperBound: %s \n", vars[query_var].name, lower_result, upper_result);
      free(lower_result);
      free(upper_result);
   }
   else
      fprintf(f, "%s\n lowerBound: %s \n upperBound: %s \n", vars[query_var].name, vars[query_var].lowerBound, vars[query_var].upperBound);

   //close Mathematica pipe
   MLPutFunction(lp, "Exit", 0L);
   MLClose(lp);
   MLDeinitialize(env);

   rc=gettimeofday(&end, NULL);
   fprintf(f, "Running time %ld ms\n", ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec))/1000);
   fclose(f);
   printf("Running time %ld ms\n", ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec))/1000);
   return 0;
}


void complete_compute(CONFIGURE *conf, MLINK lp, bounds* input){
   bounds tmp;
   if (DEBUG)
      printf("Going to finish with %s\n", input->name);

   //compute group of not contains
   if (input->nc > 0){    
      if (DEBUG) 
         printf("Compute not contains \n");
      char* nc_array[input->nc];
      from_list_to_array(input->not_contains, nc_array);
      
      count_n_not_contains(conf, lp, nc_array, input->nc, &tmp);
      if (input->lowerBound == NULL){
         copy_bounds(tmp, input);
      }
      else
         AND(conf, lp, (*input), tmp, input);
   }

   //compute group of not match regex
   if (input->nm > 0){
      if (DEBUG) 
         printf("Compute not match \n");
      char* nm_array[input->nm];
      from_list_to_array(input->not_match, nm_array);

      count_n_not_match(conf, lp, nm_array, input->nm, &tmp);
      
      if (input->lowerBound == NULL)
         copy_bounds(tmp, input);
      else
         AND(conf, lp, (*input), tmp, input);      
   }

   //compute group of not equal to const strings
   if (input->ne > 0){
      if (DEBUG) 
         printf("Compute list of not equals \n");
      char* ne_array[input->ne];
      from_list_to_array(input->not_equal, ne_array);
      compute_n_not_equal(conf, lp, ne_array, input->ne, &tmp);

      if (input->lowerBound == NULL)
         copy_bounds(tmp, input);
      else
         AND(conf, lp, (*input), tmp, input);
   }

   //compute combination of S in Ri
   if (input->n_and_match > 0){
      if (DEBUG) 
         printf("Compute list of match \n");
      char* nm_array[input->n_and_match];
      from_list_to_array(input->and_match, nm_array);
      count_n_and_match(conf, lp, nm_array, input->n_and_match, &tmp);

      if (input->lowerBound == NULL)
         copy_bounds(tmp, input);
      else
         AND(conf, lp, (*input), tmp, input);
   }

   //set initial bounds if there is no content operators before.
   if (input->lowerBound == NULL){
      if (DEBUG)
         printf("Set initial GFs\n");
      input->lowerBound  =  malloc(15);
      sprintf(input->lowerBound, "1/(1-%dX)", conf->alphabet_size);
      
      input->upperBound  =  malloc(15);
      sprintf(input->upperBound, "1/(1-%dX)", conf->alphabet_size);      
   }
   
   if (input->zero_lower_bound)
      strcpy(input->lowerBound, "0");
   /*
   For length operator
    */
   if (DEBUG)
      printf("Change the length of %s from %d to %d \n", input->name, input->lenL, input->lenU);

   //if there is any length operator which change the range of length
   if (input->lenU < conf->max_gf || input->lenL > 0){
      char* ltmp =  malloc(strlen(input->lowerBound)+1);
      strcpy(ltmp, input->lowerBound);

      char* utmp =  malloc(strlen(input->upperBound)+1);
      strcpy(utmp, input->upperBound);

      if (input->lenU < conf->max_gf){
         SUBF(conf, lp, ltmp, input->lenU, &ltmp);
         SUBF(conf, lp, utmp, input->lenU, &utmp);   
      }

      if (input->lenL > 0){
         char* tmp1;
         char* tmp2;
         
         SUBF(conf, lp, utmp, input->lenL-1, &tmp1);      
         calc(conf, lp, utmp, tmp1, &utmp, '-');      
         
         if (strcmp(ltmp, "0")){
            SUBF(conf, lp, ltmp, input->lenL-1, &tmp2);
            calc(conf, lp, ltmp, tmp2, &ltmp, '-');
            free(tmp2);
         }

         free(tmp1);      
      }
      copy_bounds_concrete(ltmp, utmp, input);
      
      free(ltmp);
      free(utmp);
   }
}

void computeOR(CONFIGURE *conf, MLINK lp, FILE *file, bounds* return_results, bounds vars[], int i){
   int code;
   int iE, nExp, upper;
   fscanf(file, "%d %d", &nExp, &upper);
   int not_case = 0;
   bounds results;

   initialize(conf, &results, i);
   results.upperBound =  malloc(15);
   strcpy(results.upperBound, "0");
   results.lowerBound =  malloc(15);
   strcpy(results.lowerBound, "0");
   results.content_opr = 1;
   results.lenL = conf->max_gf+1;
   results.lenU = -1; 

   for (iE = 0; iE < nExp; iE++){
      fscanf(file, "%d", &code);
      if (DEBUG)
         printf("%d %d", iE, code);

      //contains
      if (code == CODE_CONTAINS){
         char* pattern = malloc (MAX_PATTERN_LEN);
         fscanf(file, "%s", pattern);
         if (DEBUG) 
            printf("%d %s\n", code, pattern);
         if (not_case){            
            bounds tmp;
            count_not_contains(conf, lp, pattern, &tmp);
            OR(conf, lp, results, tmp, &results);            
         }
         else{            
            bounds tmp;
            count_contains(conf, lp, pattern, &tmp);
            OR(conf, lp, results, tmp, &results);            
         }    
      }

      //strstr
      if (code == CODE_STRSTR){
         char* pattern = malloc (MAX_PATTERN_LEN);
         int position;
         fscanf(file, "%s %d", pattern, &position);
         if (DEBUG) 
            printf("%d %s %d\n", code, pattern, position);

         bounds tmp;
         count_strstr(conf, lp, pattern, position, &tmp);
         if (not_case)
            NOT(conf, lp, tmp, &tmp);  
         OR(conf, lp, results, tmp, &results);
      }

      //equal to other variable
      if (code == CODE_EQUAL){
         char* var1 =  malloc (MAX_PATTERN_LEN);
         fscanf(file, "%s", var1);
         if (DEBUG) printf("%d %s\n", code, var1);

         int j, p1 = -1;
         for (j = 0; j < i; j++)
            if (strcmp(vars[j].name, var1) == 0)
               if (j > 0){
                  bounds tmp;
                  copy_bounds(vars[j], &tmp);
                  if (not_case)
                     NOT(conf, lp, tmp, &tmp); 
                  OR(conf, lp, results, tmp, &results);
               }
      }

      //concat
      if (code == CODE_CONCAT){
         char* var1 = malloc (MAX_PATTERN_LEN);
         char* var2 = malloc (MAX_PATTERN_LEN);
         fscanf(file, "%s %s", var1, var2);
         if (DEBUG)
            printf("%d %s %s\n", code, var1, var2);

         //find the position of p1, p2 in the array list
         int j, p1 = -1, p2 = -1;
         for (j = 0; j < i; j++){
            if (strcmp(vars[j].name, var1) == 0)
               p1 = j;
            if (strcmp(vars[j].name, var2) == 0)
               p2 = j;
         }

         //if they are computed previously (and mostly are since the concat graph)
         if (p1 >= 0 && p2 >= 0){
            char* tmp;
            char* tmp1;            
            
            calc(conf, lp, vars[p1].upperBound, vars[p2].upperBound, &tmp1, '*');
            
            if (conf->opt_concat_optimize){
               optimize_concat(conf, lp, vars[p1].lowerBound, vars[p2].lowerBound, &tmp);
            }
            else{
               calc(conf, lp, vars[p1].lowerBound, vars[p2].lowerBound, &tmp, '*');
               F1(conf, lp, tmp, &tmp);
            }

            
            bounds data;
            copy_bounds_concrete(tmp, tmp1, &data);
            if (not_case)
               NOT(conf, lp, data, &data);               
                        
            OR(conf, lp, results, data, &results);   
            free(tmp);
            free(tmp1);
         }
      }

      //length
      if (code == CODE_LENGTH){
         char* op =  malloc (2);
         fscanf(file, "%s", op);
         int number;
         fscanf(file, "%d", &number);
         if (DEBUG) 
            printf("%d %s %d\n", code, op, number);

         if (not_case)
            switch_operator(&op);

         if (strstr(op, ">="))
            results.lenL = min(results.lenL, number);
         else if (strstr(op, "<="))
            results.lenU = max(results.lenU, number);
         else if (strstr(op, "<"))
            results.lenU = max(results.lenU, number-1);
         else if (strstr(op, ">"))
            results.lenL = min(results.lenL, number+1);
         else if (strstr(op, "!=")){
            //just pass?            
         }
         else if (strstr(op, "=")){
            //TODO
         }
      }

      //match regex
      if (code == CODE_REGEX){
         char* pattern =  malloc (MAX_REGEX_LEN);
         fscanf(file, "%s", pattern);
         if (DEBUG) printf("%d %s\n", code, pattern);
         if (not_case){
            bounds tmp;
            count_not_match_regex(conf, lp, pattern, &tmp);
            OR(conf, lp, results, tmp, &results);            
         }
         else{
            results.nm += insert(results.not_match, pattern);
         }
      }

      //compare with single constString
      if (code == CODE_EQUAL_CSTR){
         char* pattern = malloc(MAX_PATTERN_LEN);
         fscanf(file, "%s", pattern);
         if (DEBUG)
            printf("Compare to %s\n", pattern);

         if (not_case){
            // != constString
            char* tmp = malloc(15);
            sprintf(tmp, "1/(1-%dX) - X^%d", conf->alphabet_size, (int) strlen(pattern));
            bounds btmp;
            copy_bounds_concrete(tmp, tmp, &btmp);            
            OR(conf, lp, results, btmp, &results);
         }
         else{
            // = constString
            results.ne += insert(results.not_equal, pattern);            
         }
      }

      //is AllString
      if (code == CODE_ALL_STR){
         if (not_case)
            continue;
         char* gz = malloc(15);      
         sprintf(gz, "1/(1-%dX)", conf->alphabet_size);      
         copy_bounds_concrete(gz, gz, &results);         
      }

      //not case
      if (code == CODE_NOT) not_case = 1;
      else not_case = 0;
   }

   complete_compute_OR(conf, lp, &results, upper);
   copy_bounds(results, return_results);
   return_results->content_opr = results.content_opr;
}

void complete_compute_OR(CONFIGURE *conf, MLINK lp, bounds* input, int upper){
   bounds tmp;

   //compute OR(conf, S match R1, S match R2, S match ....)
   if (input->nm > 0){      
      if (DEBUG)
         printf("Compute match for OR %d \n", input->nm);
      char* nm_array[input->nm+1];
      from_list_to_array(input->not_match, nm_array);
      count_n_or_match(conf, lp, nm_array, input->nm, &tmp);

      if (input->lowerBound != NULL)
         OR(conf, lp, (*input), tmp, input);
      else
         copy_bounds(tmp, input);
   }
   if (DEBUG)
      printf ("%s \n %s \n", input->lowerBound, input->upperBound);

   //compute OR(conf, S = S1, S = S2, ..., S = Sn)
   if (input->ne > 0){
      if (DEBUG)
         printf("Compute OR equals of %d \n", input->ne);
      char* ne_array[input->ne];
      from_list_to_array(input->not_equal, ne_array);
      
      compute_or_n_equal(conf, lp, ne_array, input->ne, &tmp);
      OR(conf, lp, (*input), tmp, input);
   }

   //if there is at least one length operator
   if (input->lenL <= conf->max_gf || input->lenU >= 0){
      char* tmp1 = malloc(15);
      sprintf(tmp1, "1/(1-%dX)", conf->alphabet_size);
      char* res = malloc(15);
      sprintf(res, "1/(1-%dX)", conf->alphabet_size);
      
      if (input->lenU >= 0)
         SUBF(conf, lp, tmp1, input->lenU, &res);   

      if (input->lenL <= conf->max_gf && input->lenL > 0){
         char* tmp2;
         SUBF(conf, lp, tmp1, input->lenL-1, &tmp2);
         calc(conf, lp, res, tmp2, &res, '-');
         free(tmp2);
      }
      free(tmp1);
      copy_bounds_concrete(res, res, &tmp);
      OR(conf, lp, (*input), tmp, input);
   }

   // if no handle-able operators
   if (strcmp(input->lowerBound, "0") == 0 && input->nm == 0 && input->ne == 0){
      input->lowerBound = malloc(15);
      sprintf(input->lowerBound, "1/(1-%dX)", conf->alphabet_size);
      
      input->upperBound = malloc(15);
      sprintf(input->upperBound, "1/(1-%dX)", conf->alphabet_size);
   }
   
   //if the upper bound is Gz (since we extract from the OR rules)
   if (upper){
      input->upperBound  =  malloc(15);
      sprintf(input->upperBound, "1/(1-%dX)", conf->alphabet_size);      
   }
   if (DEBUG)
      printf ("%s \n %s \n", input->lowerBound, input->upperBound);
}