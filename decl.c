#include <stdio.h>
#include <stdlib.h>
#include "smalltools.h"
#include "decl.h"

decl* decl_create(char *name, type *type, expr *value, stmt *code, decl *next) {

  decl* rval = malloc(sizeof(decl));
  rval->name = name;
  rval->type = type;
  rval->value = value;
  rval->code = code;
  rval->next = next;
  rval->symbol = NULL;

  return rval;
}

int decl_print_dot(decl *d, int* global_counter) {

  int local_counter = (*global_counter)++;
  int typer = -1;
  int valuer = -1;
  int coder = -1;
  int nextr = -1;

  printf("struct%d [label=\"{DECL|{", local_counter);

  int first = 0;
  if (d->name != NULL) {
    print_with_bar_unless_first(&first, "nme = ");
    printf("%s", d->name);
  }
  if (d->type != NULL) print_with_bar_unless_first(&first, "<f0> type");
  if (d->value != NULL) print_with_bar_unless_first(&first, "<f1> value");
  if (d->code != NULL) print_with_bar_unless_first(&first, "<f2> code");
  if (d->next != NULL) print_with_bar_unless_first(&first, "<f3> next");    

  printf("}}\"];\n");

  //rec calls
  if (d->type != NULL) {
    typer = type_print_dot(d->type, global_counter);
    printf("struct%d:f0 -> struct%d;\n", local_counter, typer);
  }
  if (d->value != NULL) {
    valuer = expr_print_dot(d->value, global_counter);
    printf("struct%d:f1 -> struct%d;\n", local_counter, valuer);
  }
  if (d->code != NULL) {
    coder = stmt_print_dot(d->code, global_counter);
    printf("struct%d:f2 -> struct%d;\n", local_counter, coder);
  }
  if (d->next != NULL) {
    nextr = decl_print_dot(d->next, global_counter);
    printf("struct%d:f3 -> struct%d;\n", local_counter, nextr);    
  }

  return local_counter;
}
