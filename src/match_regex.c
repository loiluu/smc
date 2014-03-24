#include "utils.h"
#include "match_regex.h"
#include "mathlink.h"
#include "config.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define DEBUG 0
/*
This function translates regular_expression to generating fuction.
For this translation, we support only below simple grammar of regex:
Regex := character
      | Regex Regex
      | Regex | Regex
      | Regex*
This is the hand-coded translation, for the better translation which supports
full pcre, check get_gf_from_regex() function 
 */
void get_generating_function_from_regex(CONFIGURE *conf, char* regular_expression, char** gen_regex){
   int bracket_stack[MAX_REGEX_LEN];
   char final_representation[MAX_REGEX_LEN]="";
   int top = 0;
   char regex_chars[MAX_REGEX_LEN] = ".|*()$";
   int len = strlen(regular_expression), i = 0;   
   int previous_position = 0;
   for (i = 0; i < len; i++){
      if (NULL == strchr(regex_chars, regular_expression[i]))
      {
         if (i > 0 && regular_expression[i-1] != '|' && regular_expression[i-1] != '('){
            strncat(final_representation, "*X", 2);            
         }
         else if (i==0 || regular_expression[i-1] == '('){
            strncat(final_representation, "X", 1);               
         }
         else if (regular_expression[i-1] == '|'){
            strncat(final_representation, "+X", 2);            
         }
         previous_position = strlen(final_representation) - 1;
      }
      else if (regular_expression[i] == '.'){
         char tmp[10];
         if (i > 0 && regular_expression[i-1] != '|' && regular_expression[i-1] != '('){
            sprintf(tmp, "*%dX", conf->alphabet_size);
            strcat(final_representation, tmp);        
         }
         else if (i==0 || regular_expression[i-1] == '('){
            sprintf(tmp, "%dX", conf->alphabet_size);
            strcat(final_representation, tmp);               
         }
         else if (regular_expression[i-1] == '|'){
            sprintf(tmp, "+%dX", conf->alphabet_size);
            strcat(final_representation, tmp);                          
         }
         sprintf(tmp, "%dX", conf->alphabet_size);
         previous_position = strlen(final_representation) - strlen(tmp);
      }
      else if (regular_expression[i] == '$'){
         char tmp[10];
         if (i > 0 && regular_expression[i-1] != '|' && regular_expression[i-1] != '('){
            sprintf(tmp, "*1");
            strcat(final_representation, tmp);        
         }
         else if (i==0 || regular_expression[i-1] == '('){
            sprintf(tmp, "1");
            strcat(final_representation, tmp);               
         }
         else if (regular_expression[i-1] == '|'){
            sprintf(tmp, "+1");
            strcat(final_representation, tmp);                          
         }         
         previous_position = strlen(final_representation) - 1;
      }
      else if (regular_expression[i] == '('){
         if (i > 0 && regular_expression[i-1] != '|' && regular_expression[i-1] != '('){
            strncat(final_representation, "*(", 2);  
         }
         else if ((i==0) || regular_expression[i-1] == '('){
            strncat(final_representation, "(", 1);            
         }
         else if (regular_expression[i-1] == '|'){
            strncat(final_representation, "+(", 2);
         }
         bracket_stack[top++] = strlen(final_representation)-1;          
      }
      else if (regular_expression[i] == ')'){
         strncat(final_representation, ")", 1);
         previous_position = bracket_stack[--top];
      }
      else if (regular_expression[i] == '*'){
         char new_str[MAX_REGEX_LEN] = "(1/(1-";         
         char tmp[MAX_REGEX_LEN];
         strncpy(tmp, final_representation+previous_position, strlen(final_representation)-previous_position);
         tmp[strlen(final_representation)-previous_position] = '\0';
         strncat(new_str, tmp, strlen(tmp));
         strncat(new_str, "))", 2);         
         strcpy(final_representation+previous_position, new_str);               
      }         
   }
   *gen_regex = malloc(strlen(final_representation)+1);
   strcpy(*gen_regex, final_representation);

}


/*
translates regular_expression to generating fuction for pcre format regex
@TODO: add argument to change the character size when calling pcre_tohampi.pm
 */
void get_gf_from_regex(CONFIGURE *conf, char* regular_expression, char** gen_regex){
   char* pcre_query = malloc(MAX_REGEX_LEN);
   sprintf(pcre_query, "perl run_pcre.pl \"/%s/\"", regular_expression);
   // printf("%s\n", pcre_query);
   int status = system(pcre_query);
   status = system("python read_pcre.py 0");
   free(pcre_query);

   FILE* file = fopen("gf_regex.txt", "r");
   size_t len=0;
   ssize_t read;
   read = getline(gen_regex, &len, file);
   
   fclose(file);

   //remove the temporary files
   // status=system("rm gf_regex.txt pcre_output.txt");
}

/*
This function computes the GFs for bounds of strings matching regex
 */
void count_match_regex(CONFIGURE *conf, MLINK lp, char* regular_expression, struct bounds* results){
   char* gen_regex;
   get_gf_from_regex(conf, regular_expression, &gen_regex);      
   char* gf;
   send_to_mathematica(lp, gen_regex, &gf);
   
   char* tmp;
   char* gz = malloc(20);
   sprintf(gz, "1/(1-%dX)", conf->alphabet_size);
   MINF(conf, lp, gf, gz, &tmp);
   
   (*results).upperBound = malloc(strlen(tmp)+1);
   strcpy((*results).upperBound, tmp);
   
   char* tmp2;
   F1(conf, lp, tmp, &tmp2);
   (*results).lowerBound = malloc(strlen(tmp2)+1);
   strcpy((*results).lowerBound, tmp2);

   free(tmp2);   
   free(tmp);
   free(gf);
   free(gen_regex);
}

/*
This function computes the GFs for bounds of strings matching regex
 */
void count_not_match_regex(CONFIGURE *conf, MLINK lp, char* regular_expression, struct bounds* results){
   struct bounds tmp;
   count_match_regex(conf, lp, regular_expression, &tmp);
   NOT(conf, lp, tmp, results);
}

/*
This function returns the GF of bounds for \neg S in S_i (0 <= i < n))
Convert to S \notin (S_0 | S_1 | ... | S_n-1)
*/
void count_n_not_match(CONFIGURE *conf, MLINK lp, char** s, int n, struct bounds* results){
   int i, j;
   int lenGf = 0;
   char *gf_table[n];
   if (DEBUG){      
      for (j = 0; j < n; j++) printf("String %d is %s \n", j, s[j]);
   }

   for (i = 0; i < n; i++)
   {   
      char* gen_regex;      
      get_gf_from_regex(conf, s[i], &gen_regex);
      gf_table[i] = (char *)malloc(strlen(gen_regex)+1);
      lenGf += strlen(gen_regex);
      strcpy(gf_table[i], gen_regex);
      free(gen_regex);
   }
   
   //compute generating function for R1 | R2 | R3... |Rn   
   char* new_gen = malloc(lenGf + 50 + 3*n);
   sprintf(new_gen, "1/(1-%dX)-(", conf->alphabet_size);
   for (i = 0; i < n; i++){
      char* gen_regex = malloc (strlen(gf_table[i])+5);
      sprintf(gen_regex, "+(%s)", gf_table[i]);
      strcat(new_gen, gen_regex);
      free(gen_regex);
   }
   strcat(new_gen, ")");
   if (DEBUG)
      printf("%s \n", new_gen);
   //compute lowerbound
   char* gf;
   char* tmp;
   char* minupper;
   send_to_mathematica(lp, new_gen, &gf);
   
   //make sure the lower bound is >= 0
   MAXF(conf, lp, gf, "0", &tmp);

   (*results).lowerBound = malloc(strlen(tmp)+1);
   strcpy((*results).lowerBound, tmp);

   //upper bound is Min(U1, U2, U3...)
   for (i = 0; i < n; i++){
      char* tmpi = malloc(strlen(gf_table[i]) + 30);
      char* gfi;
      sprintf(tmpi, "1/(1-%dX)-(%s)", conf->alphabet_size, gf_table[i]);
      send_to_mathematica(lp, tmpi, &gfi);
      if (DEBUG)
         printf ("%s \n", tmpi);
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
      free(gfi);
      free(tmpi);
   }

   (*results).upperBound = malloc(strlen(minupper)+1);
   strcpy((*results).upperBound, minupper);

   free(minupper);
   free(tmp);
   free(gf);
}

/*
This function returns the GF of bounds for  S in Si or ...)
*/
void count_n_or_match(CONFIGURE *conf, MLINK lp, char** s, int n, struct bounds* results){
   int i, j;
   int lenGf = 0;
   char *gf_table[n];
   if (DEBUG){
      printf ("Parameters %d\n", n);
      for (j = 0; j < n; j++) printf("String %d is %s \n", j, s[j]);
   }

   for (i = 0; i < n; i++)
   {   
      char* gen_regex;      
      get_generating_function_from_regex(conf, s[i], &gen_regex);
      gf_table[i] = (char *)malloc(strlen(gen_regex)+1);
      lenGf += strlen(gen_regex);
      strcpy(gf_table[i], gen_regex);
   }
   
   //compute generating function for R1 | R2 | R3... |Rn   
   char* new_gen = malloc(lenGf + 50 + 3*n);
   strcpy(new_gen, " ");
   for (i = 0; i < n; i++){
      char* gen_regex = malloc (strlen(gf_table[i])+5);
      sprintf(gen_regex, "+(%s)", gf_table[i]);
      strcat(new_gen, gen_regex);
      free(gen_regex);
   }
   
   char* gf;
   char* tmp;
   send_to_mathematica(lp, new_gen, &gf);
   char gz[15];
   sprintf(gz, "1/(1-%dX)", conf->alphabet_size);

   MINF(conf, lp, gf, gz, &tmp);
   (*results).upperBound = malloc(strlen(tmp)+1);
   strcpy((*results).upperBound, tmp);

   (*results).lowerBound = malloc(2);
   strcpy((*results).lowerBound, "0");
   free(tmp);
   free(gf);
}

/*
This function returns the GF of bounds for  S in R1 and S in R2 and ...)
*/
void count_n_and_match(CONFIGURE *conf, MLINK lp, char** s, int n, struct bounds* results){
   FILE* file = fopen("regex_query.input", "w");
   if (file!=NULL)
   {
      int i;
      for (i = 0; i < n; i++){
         fputs (s[i], file);
         fputs("\n", file);
      }
      fclose(file);
   }
   //call java automaton to compute the intersection of regexes
   int status = system("java -classpath \"automaton\" dk.brics.automaton.CombineRegexes");      

   FILE* ofile = fopen("regex_query.output", "r");
   if (ofile!=NULL)
   {
      char regex[MAX_REGEX_LEN];
      fgets (regex, sizeof(regex), ofile);
      strtok(regex, "\n");
      
      fclose (ofile);
      count_match_regex(conf, lp, regex, results);
   }
}