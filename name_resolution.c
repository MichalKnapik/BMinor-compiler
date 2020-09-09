#include <stdio.h>
#include <assert.h>
#include "name_resolution.h"

int error_count = 0;

void expr_resolve(expr *e) {

  if (e == NULL) return;

  if(e->kind == EXPR_NAME) {
    e->symbol = scope_lookup(e->name);
    if (e->symbol == NULL) {
      printf("Error in name resolution: unknown name %s.\n", e->name);
      error_count++;
    }
  }

  expr_resolve(e->left);
  expr_resolve(e->right);
}

void stmt_resolve(stmt *s) {

  //note: all this can be simplified, but it's easier to extend
  //if written this way
  if (s == NULL) return;

  if (s->kind == STMT_DECL) decl_resolve(s->decl);
  if (s->kind == STMT_EXPR) expr_resolve(s->expr);
  if (s->kind == STMT_IF_ELSE) {
    expr_resolve(s->expr);
    stmt_resolve(s->body);
    stmt_resolve(s->else_body);
  }
  if (s->kind == STMT_FOR) {
    //put new scope here if you want to declare vars in for loop
    expr_resolve(s->init_expr);
    //----------------------------------------------
    expr_resolve(s->expr);
    expr_resolve(s->next_expr);
    stmt_resolve(s->body);
  }
  if (s->kind == STMT_PRINT) expr_resolve(s->expr);
  if (s->kind == STMT_RETURN) expr_resolve(s->expr);
  if (s->kind == STMT_BLOCK) {
    scope_enter();
    stmt_resolve(s->body);
    scope_exit();
  }

  stmt_resolve(s->next);
}

void decl_resolve(decl *d) {

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
  scope_enter(); //a new scope to allow shadowing function parameters
  stmt_resolve(d->code);
  scope_exit(); 
  scope_exit();

  decl_resolve(d->next);
}

void param_list_resolve(param_list* l) {

  int paramctr = 0;

  while (l != NULL) {
    if (scope_lookup_current(l->name) != NULL) {
      printf("Error: duplicate parameter %s.\n", l->name);
      ++error_count;
    }
    l->symbol = symbol_create(SYMBOL_PARAM, l->type, l->name);
    l->symbol->which = paramctr++;

    scope_bind(l->name, l->symbol);
    l = l->next;
  }

}

void mark_symbols_with_numbers_decl(decl* d, int* symbol_count) {

  if (d == NULL) return;

  if (d->code != NULL) { //diving into a function
    *symbol_count = 0;
    mark_symbols_with_numbers_type(d->type, symbol_count);
    *symbol_count = 0;    
    mark_symbols_with_numbers_stmt(d->code, symbol_count);
  }

  //local var decl
  if (d->symbol->kind == SYMBOL_LOCAL) {
    if (d->symbol->which < 0) { //marking new symbol
      d->symbol->which = (*symbol_count)++;
      mark_symbols_with_numbers_expr(d->value, symbol_count);
    }
  }

  mark_symbols_with_numbers_decl(d->next, symbol_count);
}

void mark_symbols_with_numbers_stmt(stmt* s, int* symbol_count) {
  
  if (s == NULL) return;

  mark_symbols_with_numbers_decl(s->decl, symbol_count);
  mark_symbols_with_numbers_expr(s->init_expr, symbol_count);
  mark_symbols_with_numbers_expr(s->expr, symbol_count);
  mark_symbols_with_numbers_expr(s->next_expr, symbol_count);
  mark_symbols_with_numbers_stmt(s->body, symbol_count);  
  mark_symbols_with_numbers_stmt(s->else_body, symbol_count);
  mark_symbols_with_numbers_stmt(s->next, symbol_count);

}

void mark_symbols_with_numbers_expr(expr* e, int* symbol_count) {
  
  if (e == NULL) return;

  if (e->kind == EXPR_NAME && e->symbol != NULL && e->symbol->which < 0) e->symbol->which = (*symbol_count)++;

  mark_symbols_with_numbers_expr(e->left, symbol_count);  
  mark_symbols_with_numbers_expr(e->right, symbol_count);  

}

void mark_symbols_with_numbers_type(type* t, int* symbol_count) {
  if (t == NULL) return;
  mark_symbols_with_numbers_params(t->params, symbol_count);
}

void mark_symbols_with_numbers_params(param_list* p, int* symbol_count) {

  if (p == NULL) return;

  for (param_list* pl = p; pl != NULL; pl = pl->next) {
          pl->symbol->which = (*symbol_count)++;
  }

}

void mark_program_symbols_with_numbers(decl* d) {
  if (d == NULL) return;
  int symbol_count = 0;
  mark_symbols_with_numbers_decl(d, &symbol_count);
}
