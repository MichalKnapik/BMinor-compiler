#include <stdio.h>
#include <stdlib.h>
#include "stmt.h"
#include "decl.h"
#include "expr.h"

stmt* stmt_create(stmt_t kind, decl *decl, expr *init_expr, expr *exprf, expr *next_expr, stmt *body, stmt *else_body, stmt *next) {

  stmt* rval = malloc(sizeof(stmt));
  rval->decl = decl;
  rval->init_expr = init_expr;
  rval->expr = exprf;
  rval->next_expr = next_expr;
  rval->body = body;
  rval->else_body = else_body;
  rval->next = next;

  return rval;
}
