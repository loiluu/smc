#ifndef UTILS_H_   /* Include guard */
#define UTILS_H_

#include <stdio.h>
#include "string.h"
#include "list.h"
#include "mathlink.h"
#include "config.h"
#include <stdlib.h>
#include <errno.h>
/*
This is the data structure to represent the bounds of variables
 */
typedef struct bounds {
   char* name;
   char* lowerBound;
   char* upperBound;
   //the list of strings that this variable doesn't contain
   stringList* not_contains;

   //the list of regex that this variable doesn't match
   stringList* not_match;

   //the list of regex that this variable doesn't match
   stringList* and_match;

   //the list of string that this variable doesn't equal
   stringList* not_equal;

   //has content operator
   int content_opr;   

   //get zero_lower_bound
   int zero_lower_bound;

   //concat list
   intList* concat_list;

   //number of elements in concat_list;
   int nconcat;

   //no. of not contains
   int nc;
   //no. of not match regex
   int nm;
   //no. of match regex
   int n_and_match;
   //no. of not equal string
   int ne;
   //lower bound of length
   int lenL;
   //upper bound of length
   int lenU;
} bounds;

typedef struct CONFIGURE {
   //maximum length of gf
   int max_gf;

   //Number of characters in space
   int alphabet_size;

   //Concat optimize option
   int opt_concat_optimize;

   //Number of interator for concat, the greater the better (but slower)
   int concat_iters;
} CONFIGURE;

void initialize(CONFIGURE *conf, bounds* b, int n);

/*
Send request expression, normalize it 
 */

void send_to_mathematica(MLINK lp, char *expression, char** result);

/*
Calculate MINF function
 */
void MINF(CONFIGURE *conf, MLINK lp, char* expresison1, char* expression2, char** result);

/*
Calculate MAXF function
 */
void MAXF(CONFIGURE *conf, MLINK lp, char* expression1, char* expression2, char** result);


/*
Calculate TRUNC/SUBF function
 */
void SUBF(CONFIGURE *conf, MLINK lp, char* poly, int number, char** result);

/*
Calculate F1/DEDUP
 */
void F1(CONFIGURE *conf, MLINK lp, char* P, char** result);

/*
Calculate other operator like +, -, *, /
 */
void calc(CONFIGURE *conf, MLINK lp, char* expression1, char* expression2, char** result, char opr);

/*
Calculate not operator
 */
char* NOT(CONFIGURE *conf, MLINK lp, bounds input, bounds* results);

/*
Calculate the coefficient a_number
 */
void get_coef(CONFIGURE *conf, MLINK lp, char* expression, int number, char** result);

/*
Calculate the conjunction between two conditions
 */
void AND(CONFIGURE *conf, MLINK lp, bounds input1, bounds input2, bounds* results);


/*
Calculate the disjunction between two conditions
 */
void OR(CONFIGURE *conf, MLINK lp, bounds input1, bounds input2, bounds* results);

/*
Get lower bound for res = p1.p2
 */
void optimize_concat(CONFIGURE *conf, MLINK lp, char* p1, char* p2, char** res);
/*
Copy lower bound and upper bound, other fields of results keep the same
 */
void copy_bounds(bounds input, bounds* results);

/*
Copy lower bound and upper bound from strings, other fields of results keep the same
 */
void copy_bounds_concrete(char* lower, char* upper, bounds* results);

/*
Compute S != S1, S != S2, ..., S != Sn
 */
void compute_n_not_equal(CONFIGURE *conf, MLINK lp, char** s, int n, bounds* result);

/*
Compute S = S1 or S = S2 or ... or S = Sn
 */
void compute_or_n_equal(CONFIGURE *conf, MLINK lp, char** s, int n, bounds* result);
/*
switch the operator
 */
void switch_operator(char** op);

/**
 * get the max value of two numbers
 * @param  x  first number
 * @param  y second number
 * @return   max value
 */
int max(int x, int y);

int min(int x, int y);

/**
 * get absolute value of number x
 * @param  x [the number]
 * @return   [the absoulute value of x]
 */
double get_abs(double x);

/*
This function builds the auto correlation table of string s
and stores the result into the table ctable
*/
void compute_correlation_table(char* s, int ctable[], int size);


/*
This function builds the reverse correlation table of string s
and stores the result into the table reverse_ctable
*/
void compute_reverse_correlation_table(char* s, int reverse_ctable[], int size);


/*
The aim of preprocess are to get the bounds of length for all variables 
and decide which variable should have the lower bound of 0
 */
void preprocess(CONFIGURE *conf, MLINK lp, FILE *file, bounds var[], int nVar);

void make_simple_call(MLINK lp);

/*
Read configuration from smc.conf
 */
void read_configuration(CONFIGURE *conf);
/*
Print the configuration
 */
void print_configuration(CONFIGURE *conf);
#endif

