#ifndef CONFIG_H_   /* Include guard */
#define CONFIG_H_

//maximum of regex len
#define MAX_REGEX_LEN 100000

//maximum length of const string or pattern
#define MAX_PATTERN_LEN 100

//code of intermediate representation of constraint
#define CODE_NEW_VAR 0
#define CODE_NOT 1
#define CODE_OR 2
#define CODE_CONTAINS 3
#define CODE_STRSTR 4
#define CODE_EQUAL 5
#define CODE_CONCAT 6
#define CODE_LENGTH 7
#define CODE_REGEX 8
#define CODE_EQUAL_CSTR 9
#define CODE_ALL_STR 10
#define CODE_QUERY 11

#endif
