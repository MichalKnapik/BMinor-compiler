#include <stdio.h>
#include <stdlib.h>
#include "smalltools.h"
#include "type.h"

type* type_create(type_t kind, type *subtype, param_list *params) {

  type* rval = malloc(sizeof(type));
  rval->kind = kind;
  rval->params = params;
  rval->subtype = subtype;

  return rval;
}

int type_print_dot(type *t, int* global_counter) {

  int local_counter = (*global_counter)++;
  int paramr = -1;
  int subtypr = -1;

  const char* typenames [] = {"TYPE_VOID", "TYPE_BOOLEAN", "TYPE_CHARACTER", "TYPE_INTEGER",
			      "TYPE_STRING", "TYPE_ARRAY", "TYPE_FUNCTION"};
  const char* kindr = typenames[t->kind - TYPE_VOID];

  printf("struct%d [label=\"", local_counter);
  printf("{%s|{", kindr);

  int first = 0;
  if (t->params != NULL) print_with_bar_unless_first(&first, "<f0> params");
  if (t->subtype != NULL) print_with_bar_unless_first(&first, "<f1> subtype");

  printf("}}\"];\n");

  //rec calls
  if (t->params != NULL) {
    paramr = param_print_dot(t->params, global_counter);
    printf("struct%d:f0 -> struct%d;\n", local_counter, paramr);    
  }
  if (t->subtype != NULL) {
    subtypr = type_print_dot(t->subtype, global_counter);
    printf("struct%d:f1 -> struct%d;\n", local_counter, subtypr);        
  }

  return local_counter;
}
