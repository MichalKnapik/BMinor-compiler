#include <stdio.h>
#include <stdlib.h>
#include "decl.h"

decl* decl_create(char *name, type *type, expr *value, stmt *code, decl *next) {

  decl* rval = malloc(sizeof(decl));
  rval->name = name;
  rval->type = type;
  rval->value = value;
  rval->code = code;
  rval->next = next;
  
  return rval;
}

int decl_print_dot(decl *d, int* global_counter) {

  int local_counter = (*global_counter)++;
  int typer = -1;
  int valuer = -1;
  int coder = -1;
  int nextr = -1;

  printf("struct%d [", local_counter);
  if (d->name != NULL) {
    printf("label=\"{DECL|{<f0> name");
  }
  if (d->type != NULL) {
    printf("|<f1> type");
    typer = type_print_dot(d->type, global_counter);
  }
  if (d->value != NULL) {
    printf("|<f2> value");
    valuer = expr_print_dot(d->value, global_counter);
  }
  if (d->code != NULL) {
    printf("|<f3> code");
    coder = stmt_print_dot(d->code, global_counter);
  }
  if (d->next != NULL) {
    nextr = decl_print_dot(d->next, global_counter);
    printf("|<f4> next");    
  }

  printf("\"}}];");

  //todo - print transitions

  return local_counter;
}
