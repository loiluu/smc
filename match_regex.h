#ifndef MATCH_REGEX_H_   /* Include guard */
#define MATCH_REGEX_H_

#include "utils.h"
#include "mathlink.h"

/*
This function counts the GFs for bounds of strings matching ther regular expression
 */
void count_match_regex(CONFIGURE *conf, MLINK lp, char* regular_expression, struct bounds* results);

/*
This function counts the GFs for bounds of strings not matching the regular expression
 */
void count_not_match_regex(CONFIGURE *conf, MLINK lp, char* regular_expression, struct bounds* results);


/*
This function returns the GF of bounds for \neg S.match(R_i)
*/
void count_n_not_match(CONFIGURE *conf, MLINK lp, char** s, int n, struct bounds* results);

/*
This function returns the GF of bounds for S.match(R1) or S.match(R2) or ...
 */
void count_n_or_match(CONFIGURE *conf, MLINK lp, char** s, int n, struct bounds* results);

/*
This function returns the GF of bounds for S.match(R1) and S.match(R2) and ...
 */
void count_n_and_match(CONFIGURE *conf, MLINK lp, char** s, int n, struct bounds* results);

#endif