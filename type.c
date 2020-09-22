#include <stdio.h>
#include <stdlib.h>
#include "smalltools.h"
#include "type.h"

type* type_create(type_t kind, type *subtype, param_list *params) {

  type* rval = malloc(sizeof(type));
  rval->kind = kind;
  rval->params = params;
  rval->subtype = subtype;
  rval->arrsize = 1;

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
  if (t->arrsize >= 0 && t->kind == TYPE_ARRAY) {
    print_with_bar_unless_first(&first, " size = ");
    printf("%d", t->arrsize);
  }

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

int is_basic(type* a) {
  return (a->kind == TYPE_BOOLEAN || a->kind == TYPE_CHARACTER || a->kind == TYPE_INTEGER || a->kind == TYPE_STRING);
}

int type_equals(type *a, type *b) {

  if (a->kind == b->kind) {
    if (is_basic(a)) return 1;
    if (a->kind == TYPE_ARRAY) return type_equals(a->subtype, b->subtype);
    if (a->kind == TYPE_FUNCTION) {

      param_list* pla = a->params;
      param_list* plb = b->params;

      while (1) {
	if (pla == NULL && plb == NULL) break;
	else if ((pla != NULL && plb == NULL) || (pla == NULL && plb != NULL)) return 0;
	if (!type_equals(pla->type, plb->type)) return 0;

	pla = pla->next;
	plb = plb->next;	
      }

      return type_equals(a->subtype, b->subtype);
    }

  }

  return 0;
}

type* type_copy(type *t) {
  if (t == NULL) return NULL;
  return type_create(t->kind, type_copy(t->subtype), param_list_copy(t->params));
}

void type_delete(type *t) {

  if (t == NULL) return;

  param_list_delete(t->params);
  type_delete(t->subtype);
  free(t);
}

int type_size(type *t) {

  if (t->kind == TYPE_BOOLEAN || t->kind == TYPE_CHARACTER) return 1;
  if (t->kind == TYPE_VOID) return 0;

  //other cases are either int or refs
  return 8;
}

void print_type(type* t) {
  char* tnames[] = {
	      "TYPE_VOID",
	      "TYPE_BOOLEAN",
	      "TYPE_CHARACTER",
	      "TYPE_INTEGER",
	      "TYPE_STRING",
	      "TYPE_ARRAY",
	      "TYPE_FUNCTION"};

    printf("%s\n", tnames[t->kind - TYPE_VOID]);
}
