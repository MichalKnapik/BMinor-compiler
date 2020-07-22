#include <stdio.h>
#include <stdlib.h>
#include "param_list.h"
#include "type.h"

param_list* param_list_create(char *name, type *type, param_list *next) {

  param_list* rval = malloc(sizeof(param_list));
  rval->name = name;
  rval->type = type;
  rval->next = next;
  rval->symbol = NULL;

  return rval;
}

int param_print_dot(param_list* l, int* global_counter) {
  
  int local_counter = (*global_counter)++;  
  int typer = -1;
  int nextr = -1;

  printf("struct%d [", local_counter);

  if (l->name != NULL) printf("label=\"{TYPE|{ nme = %s", l->name);
  if (l->type != NULL) printf("|<f0> next");    
  if (l->next != NULL) printf("|<f1> next");    

  printf("\"}}];\n");

  //rec calls
  if (l->type != NULL) typer = type_print_dot(l->type, global_counter);
  if (l->next != NULL) nextr = param_print_dot(l->next, global_counter);

  //todo - print transitions

  return local_counter;
}
