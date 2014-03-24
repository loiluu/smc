#include "string.h"
#include "list.h"
#include <stdio.h>
#include <stdlib.h>
//get number of elements in list
int insert(stringList* list, char* string){
   stringList* current = list;

   while (current->next != NULL){
      current = current->next;
      if (strcmp(current->val, string) == 0)
         return 0;      
   }
   
   stringList *newNode = (stringList*)malloc(sizeof(stringList));

   newNode->val = malloc(strlen(string)+1);
   strcpy(newNode->val, string);
   newNode->next = NULL;
   current->next = newNode;
   return 1;
}

//convert from string list to array of strings
void from_list_to_array(stringList* list, char** array){
   stringList* current = list->next;
   int i = 0;
   while (current != NULL){
      array[i++] = current->val;
      current = current->next;
   }
}

//insert new element to list
int insert_int(intList* list, int element){
   intList* current = list;

   while (current->next != NULL)
      current = current->next;      
   
   intList *newNode = (intList*)malloc(sizeof(intList));

   newNode->val = element;
   newNode->next = NULL;
   current->next = newNode;
   return 1;
}

void from_int_list_to_array(intList* list, int* array){
   intList* current = list->next;
   int i = 0;
   while (current != NULL){
      array[i++] = current->val;
      current = current->next;
   }
}