#include <stdio.h>
#include <stdlib.h>
#include "param_list.h"
#include "smalltools.h"
#include "type.h"
#include "type_check.h"
#include "scope.h"

extern int error_count;

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

  printf("struct%d [label=\"{param_list|{ ", local_counter);
  int first = 0;
  if (l->name != NULL) {
    printf("nme = %s", l->name);
    first = 1;
  }
  if (l->type != NULL) print_with_bar_unless_first(&first, "<f0> type");
  if (l->next != NULL) print_with_bar_unless_first(&first, "<f1> next");    

  printf("}}\"];\n");

  //rec calls
  if (l->type != NULL) {
    typer = type_print_dot(l->type, global_counter);
    printf("struct%d:f0 -> struct%d;\n", local_counter, typer);
  }
  if (l->next != NULL) {
    nextr = param_print_dot(l->next, global_counter);
    printf("struct%d:f1 -> struct%d;\n", local_counter, nextr);    
  }

  return local_counter;
}

void param_list_resolve(param_list* l) {

  while (l != NULL) {
    if (scope_lookup_current(l->name) != NULL) {
      printf("Error: duplicate parameter %s.\n", l->name);
      ++error_count;
    }
    l->symbol = symbol_create(SYMBOL_PARAM, l->type, l->name);
    scope_bind(l->name, l->symbol);
    l = l->next;
  }

}

param_list* param_list_copy(param_list* l) {

  if (l == NULL) return NULL;

  param_list* head = param_list_create(l->name, type_copy(l->type), NULL);
  param_list* curr = head;

  while (l->next != NULL) {
    l = l->next;
    curr->next = param_list_create(l->name, type_copy(l->type), NULL);
    curr = curr->next;
  }

  return head;
}

void param_list_delete(param_list* l) {
  if (l == NULL) return;
  type_delete(l->type);
  param_list_delete(l->next);
  free(l);
}
