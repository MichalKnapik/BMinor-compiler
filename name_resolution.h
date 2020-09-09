#ifndef NAME_RESOLUTION_H
#define NAME_RESOLUTION_H

#include "type.h"
#include "stmt.h"
#include "decl.h"
#include "expr.h"
#include "scope.h"
#include "param_list.h"

void expr_resolve(expr *e);
void stmt_resolve(stmt *s);
void decl_resolve(decl *d);
void param_list_resolve(param_list* l);

/* Mostly useful for debugging - can be called on some incorrectly typed programs because of some redundancy. */
void mark_program_symbols_with_numbers(decl* d);
void mark_symbols_with_numbers_decl(decl* d, int* symbol_count);
void mark_symbols_with_numbers_stmt(stmt* s, int* symbol_count);
void mark_symbols_with_numbers_expr(expr* e, int* symbol_count);
void mark_symbols_with_numbers_type(type* t, int* symbol_count);
void mark_symbols_with_numbers_params(param_list* p, int* symbol_count);

#endif
