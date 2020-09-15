#include <stdio.h>
#include "debug.h"
#include "codegen.h"

void print_mem_pos_decl(decl* d) {
  
  if (d == NULL) return;
  
  if (d->type->kind == TYPE_FUNCTION) {
    printf("function: %s\n", symbol_codegen(d->symbol, 0));
    print_mem_pos_params(d->type->params);
    print_mem_pos_stmt(d->code);
  }

  else if (d->symbol->kind == SYMBOL_GLOBAL) {
    printf("global var: %s\n", symbol_codegen(d->symbol, 0));
  }

  else if (d->symbol->kind == SYMBOL_LOCAL) {
    printf("\tlocal var %s in %s\n", d->symbol->name, symbol_codegen(d->symbol, 0));
  }

  print_mem_pos_decl(d->next);
}

void print_mem_pos_params(param_list* p) {

  if (p == NULL) return;

  for (param_list* pl = p; pl != NULL; pl = pl->next) {
    printf("\tparam defn: %s in %s\n", pl->name, symbol_codegen(pl->symbol, 0));
  }

}

void print_mem_pos_stmt(stmt* s) {

  if (s == NULL) return;

  print_mem_pos_decl(s->decl);
  print_mem_pos_stmt(s->body);  
  print_mem_pos_stmt(s->else_body);
  print_mem_pos_stmt(s->next);

}
