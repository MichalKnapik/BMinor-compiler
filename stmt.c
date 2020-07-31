#include <stdio.h>
#include <stdlib.h>
#include "smalltools.h"
#include "stmt.h"
#include "decl.h"
#include "expr.h"

stmt* stmt_create(stmt_t kind, decl *decl, expr *init_expr, expr *exprf, expr *next_expr,
		  stmt *body, stmt *else_body, stmt *next) {

  stmt* rval = malloc(sizeof(stmt));
  rval->kind = kind;
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

  const char * stmtnames [] = {"STMT_DECL", "STMT_EXPR", "STMT_IF_ELSE", "STMT_FOR",
			      "STMT_PRINT", "STMT_RETURN", "STMT_BLOCK"};
  const char* kindr = stmtnames[s->kind - STMT_DECL];

  printf("struct%d [label=\"", local_counter);
  printf("{%s|{", kindr);

  int first = 0;
  if (s->decl != NULL) print_with_bar_unless_first(&first, "<f0> decl");
  if (s->init_expr != NULL) print_with_bar_unless_first(&first, "<f1> init_expr");
  if (s->expr != NULL) print_with_bar_unless_first(&first, "<f2> expr"); 
  if (s->next_expr != NULL) print_with_bar_unless_first(&first, "<f3> next_expr");
  if (s->body != NULL) print_with_bar_unless_first(&first, "<f4> body");
  if (s->else_body != NULL) print_with_bar_unless_first(&first, "<f5> else_body");
  if (s->next != NULL) print_with_bar_unless_first(&first, "<f6> next");

  printf("}}\"];\n");

  //rec calls
  if (s->decl != NULL) {
    declr = decl_print_dot(s->decl, global_counter);
    printf("struct%d:f0 -> struct%d;\n", local_counter, declr);
  }
  if (s->init_expr != NULL) {
    init_exprr = expr_print_dot(s->init_expr, global_counter);
    printf("struct%d:f1 -> struct%d;\n", local_counter, init_exprr);
  }
  if (s->expr != NULL) {
    exprr = expr_print_dot(s->expr, global_counter);
    printf("struct%d:f2 -> struct%d;\n", local_counter, exprr);
  }
  if (s->next_expr != NULL) {
    next_exprr = expr_print_dot(s->next_expr, global_counter);
    printf("struct%d:f3 -> struct%d;\n", local_counter, next_exprr);
  }
  if (s->body != NULL) {
    bodyr = stmt_print_dot(s->body, global_counter);
    printf("struct%d:f4 -> struct%d;\n", local_counter, bodyr);
  }
  if (s->else_body != NULL) {
    else_bodyr = stmt_print_dot(s->else_body, global_counter);
    printf("struct%d:f5 -> struct%d;\n", local_counter, else_bodyr);
  }
  if (s->next != NULL) {
    nextr = stmt_print_dot(s->next, global_counter);
    printf("struct%d:f6 -> struct%d;\n", local_counter, nextr);    
  }
  
  return local_counter;
}

void stmt_resolve(stmt *d) {

  //todo now

}
