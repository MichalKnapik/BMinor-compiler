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

int stmt_print_dot(stmt* s, int* global_counter) {

  int local_counter = (*global_counter)++;
  int declr = -1;
  int init_exprr = -1;
  int exprr = -1;  
  int next_exprr = -1;  
  int bodyr = -1;
  int else_bodyr = -1;
  int nextr = -1;

  const char * stmtnames[] = {"STMT_DECL", "STMT_EXPR", "STMT_IF_ELSE", "STMT_FOR",
			      "STMT_PRINT", "STMT_RETURN", "STMT_BLOCK"};
  const char* kindr = stmtnames[s->kind - STMT_DECL];

  printf("struct%d [ label=\"", local_counter);
  printf("{%s|{", kindr);

  if (s->decl != NULL) {
    printf("|<f0> decl");
    declr = decl_print_dot(s->decl, global_counter);
  }
  if (s->init_expr != NULL) {
    printf("|<f1> init_expr");
    init_exprr = expr_print_dot(s->init_expr, global_counter);
  }
  if (s->expr != NULL) {
    printf("|<f2> expr");
    exprr = expr_print_dot(s->expr, global_counter);
  }
  if (s->next_expr != NULL) {
    printf("|<f3> next_expr");
    next_exprr = expr_print_dot(s->next_expr, global_counter);
  }
  if (s->body != NULL) {
    printf("|<f4> body");
    bodyr = stmt_print_dot(s->body, global_counter);
  }
  if (s->else_body != NULL) {
    printf("|<f5> else_body");
    else_bodyr = stmt_print_dot(s->else_body, global_counter);
  }

  if (s->next != NULL) {
    printf("|<f6> next");
    nextr = stmt_print_dot(s->next, global_counter);
  }

  printf("\"}}];");

  //todo - print transitions

  return local_counter;
}
