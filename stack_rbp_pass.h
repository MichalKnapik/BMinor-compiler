#ifndef STACK_RBP_PASS_H
#define STACK_RBP_PASS_H

#include "type.h"
#include "stmt.h"
#include "decl.h"
#include "expr.h"
#include "scope.h"
#include "param_list.h"

/* Should be called on correctly typechecked programs with names resolved. */
void mark_program_symbols_with_rbppos(decl* d);
void mark_symbols_with_rbppos_decl(decl* d, int* current_rbp_offset);
void mark_symbols_with_rbppos_stmt(stmt* s, int* current_rbp_offset);
void mark_symbols_with_rbppos_type(type* t, int* current_rbp_offset);

/* Fixes for parser's deficiencies. */
void fix_string_sizes_decl(decl* d);
void fix_string_sizes_stmt(stmt* s);

#endif
