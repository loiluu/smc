#ifndef CONTAINS_H_   /* Include guard */
#define CONTAINS_H_
#include "mathlink.h"
#include "utils.h"

/*
This function returns the GF of bounds for False = S.contains(s)
*/
void count_not_contains(CONFIGURE *conf, MLINK lp, char* s, struct bounds* results);

/*
This function returns the GF of bounds for True = S.contains(s)
*/
void count_contains(CONFIGURE *conf, MLINK lp, char* s, struct bounds* results);

/*
This function returns the GF of bounds for False = S.contains(s_i)
*/
void count_n_not_contains(CONFIGURE *conf, MLINK lp, char** s, int n, struct bounds* results);

/*
This function returns the GF of bounds for position = S.strstr(s)
*/
void count_strstr(CONFIGURE *conf, MLINK lp, char* pattern, int position, struct bounds* results);
#endif