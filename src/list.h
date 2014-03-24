#ifndef STRING_LIST_H_   /* Include guard */
#define STRING_LIST_H_

typedef struct node
{
  char* val;
  struct node *next;
} stringList;

typedef struct intnode
{
  int val;
  struct intnode *next;
} intList;

//insert new element to list
int insert(stringList* list, char* string);

//convert stringList to array
void from_list_to_array(stringList* list, char** array);

//insert new element to list
int insert_int(intList* list, int element);

//convert intList to array
void from_int_list_to_array(intList* list, int* array);
#endif