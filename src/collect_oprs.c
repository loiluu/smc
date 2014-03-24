#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]){
      
   char input_file[100] = "input.txt";
   int nVar;
   if (argc == 2)
      strcpy(input_file, argv[1]);

   FILE *file;
   //get the length bounds
   file = fopen(input_file, "r");  
   fscanf(file, "%d", &nVar);
   bounds vars[nVar];
   preprocess(lp, file, vars, nVar);      
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
            complete_compute(lp, &vars[i]);         
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
               count_contains(lp, pattern, &vars[i], alphabet_size);
            else{
               bounds tmp;
               count_contains(lp, pattern, &tmp, alphabet_size);
               AND(lp, vars[i], tmp, &vars[i]);
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
         count_strstr(lp, pattern, position, &tmp, alphabet_size);         
         if (not_case)
            NOT(lp, tmp, &tmp, alphabet_size);

         if (vars[i].lowerBound == NULL)
            copy_bounds(tmp, &vars[i]);
         else
            AND(lp, vars[i], tmp, &vars[i]);
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
               NOT(lp, tmp, &tmp, alphabet_size); 
               AND(lp, vars[i], tmp, &vars[i]);
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
                        
            Calc(lp, vars[p1].upperBound, vars[p2].upperBound, &tmp1, '*', alphabet_size);
            
            if (OPT_CONCAT_OPTIMIZE){
               optimize_concat(lp, vars[p1].lowerBound, vars[p2].lowerBound, &tmp, alphabet_size);
            }
            else{
               Calc(lp, vars[p1].lowerBound, vars[p2].lowerBound, &tmp, '*', alphabet_size);
               F1(lp, tmp, &tmp);
            }
            
            bounds data;
            copy_bounds_concrete(tmp, tmp1, &data);
            if (not_case)
               NOT(lp, data, &data, alphabet_size);               
            
            if (vars[i].lowerBound == NULL)
               copy_bounds(data, &vars[i]);
            else
               AND(lp, vars[i], data, &vars[i]);
            
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
            if (vars[i].lowerBound == NULL)
               count_match_regex(lp, pattern, &vars[i], alphabet_size);
            else{
               bounds tmp;
               count_match_regex(lp, pattern, &tmp, alphabet_size);
               AND(lp, vars[i], tmp, &vars[i]);
            }
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
            sprintf(tmp, "X^%d", strlen(pattern));
            bounds btmp;
            copy_bounds_concrete(tmp, tmp, &btmp);
            if (vars[i].lowerBound == NULL)
               copy_bounds(btmp, &vars[i]);
            else
               AND (lp, vars[i], btmp, &vars[i]);            
         }
      }

      //is AllString
      if (code == CODE_ALL_STR){
         char* gz = malloc(15);      
         sprintf(gz, "1/(1-%dX)", alphabet_size);      
         copy_bounds_concrete(gz, gz, &vars[i]);         
      }

      //not case
      if (code == CODE_NOT) not_case = 1;
      else not_case = 0;

      //list of or, doing the same thing again
      if (code == CODE_OR){
         bounds result;
         computeOR(lp, file, &result, vars, i);
         if (vars[i].lowerBound == NULL)
            copy_bounds(result, &vars[i]);
         else
            AND(lp, vars[i], result, &vars[i]);
         vars[i].content_opr = result.content_opr;
      }

      if (code == CODE_QUERY){
         query_var = i;
         fscanf(file, "%d", &query_len);
      }
   }
      
   complete_compute(lp, &vars[nVar-1]);
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
      get_coef(lp, vars[query_var].lowerBound, query_len, &lower_result);
      get_coef(lp, vars[query_var].upperBound, query_len, &upper_result);
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
   fprintf(f, "Running time %ld\n", ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));
   fclose(f);
   printf("Running time %ld\n", ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));
   return 0;
}
