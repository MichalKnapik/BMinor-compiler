#include <stdio.h>
#include <stdlib.h>
#include "smalltools.h"
#include "decl.h"
#include "scope.h"

extern int error_count;

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

void decl_resolve(decl *d) {

  printf("**(decl)**\n");
  print_scope();
  printf("**********\n");

  if (d == NULL) return;

  //re-declaration error
  symbol* lookup = scope_lookup_current(d->name);
  if (lookup != NULL) { 
    //current decl is the implementation of earlier prototype
    if (!(scope_level() == 1 && d->type->kind == lookup->type->kind &&
	  lookup->type->kind == TYPE_FUNCTION && d->code != NULL &&
	  lookup->flags != FUN_DEF)) {
      printf("Error in name resolution: re-declaration of %s.\n", d->name);
      error_count++;      
    }
  }

  symbol_t kind = scope_level() > 1 ? SYMBOL_LOCAL: SYMBOL_GLOBAL;

  d->symbol = symbol_create(kind, d->type, d->name);
  expr_resolve(d->value);
  scope_bind(d->name, d->symbol);

  if (d->code != NULL) d->symbol->flags = FUN_DEF;

  scope_enter();
  param_list_resolve(d->type->params);
  scope_enter();//a new scope to allow shadowing function parameters
  stmt_resolve(d->code);
  scope_exit();//---------------------------------------------------
  scope_exit();

  decl_resolve(d->next);
}
