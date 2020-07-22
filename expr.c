#include <stdio.h>
#include <stdlib.h>
#include "expr.h"

expr* expr_create(expr_t kind, struct expr *left, expr *right) {

  expr* rval = malloc(sizeof(expr));
  rval->kind = kind;
  rval->left = left;  
  rval->right = right;  

  rval->name = NULL;
  rval->literal_value = 0;
  rval->string_literal = NULL;
  rval->symbol = NULL;

  return rval;
}

expr* expr_create_name(const char *n) {

  expr* rval = expr_create(EXPR_NAME, NULL, NULL);
  rval->name = n;

  return rval;
}

expr* expr_create_integer_literal(int c) {

  expr* rval = expr_create(EXPR_INT, NULL, NULL);
  rval->literal_value = c;

  return rval;
}

expr* expr_create_boolean_literal(int c) {

  expr* rval = expr_create(EXPR_BOOL, NULL, NULL);
  rval->literal_value = c;

  return rval;
}

expr* expr_create_char_literal(char c) {

  expr* rval = expr_create(EXPR_CHAR, NULL, NULL);  
  rval->literal_value = c;

  return rval;
}

expr* expr_create_string_literal(const char *str) {

  expr* rval = expr_create(EXPR_STR, NULL, NULL);  
  rval->string_literal = str;

  return rval;
}

int expr_print_dot(expr* s, int* global_counter) {

  int local_counter = (*global_counter)++;
  int leftr = -1;
  int rightr = -1;

  const char * exprnames [] = {"EXPR_NAME", "EXPR_STR", "EXPR_INT", "EXPR_CHAR", "EXPR_BOOL", "EXPR_ARG",
			      "EXPR_ARR_SUBS", "EXPR_FUN_CALL", "EXPR_UN_MIN", "EXPR_NEG", "EXPR_EXP", "EXPR_MUL",
			      "EXPR_DIV", "EXPR_MOD", "EXPR_ADD", "EXPR_SUB", "EXPR_INC", "EXPR_DEC", "EXPR_LE",
			      "EXPR_LT", "EXPR_EQ", "EXPR_GT", "EXPR_GE", "EXPR_NEQ", "EXPR_AND", "EXPR_OR",
			      "EXPR_ASSGN"};

  const char* kindr = exprnames[s->kind - EXPR_NAME];

  printf("struct%d [ label=\"", local_counter);
  printf("{%s|{", kindr);

  if (s->name != NULL) printf("| nme = %s ", s->name);
  if (s->kind == EXPR_INT || s->kind == EXPR_CHAR || s->kind == EXPR_BOOL) printf("| lvl = %d ", s->literal_value);    
  if (s->string_literal != NULL) printf("| str = %s ", s->string_literal);
  if (s->left != NULL) printf("| <f0> left");
  if (s->right != NULL) printf("| <f1> right");

  printf("\"}}];\n");

  //rec calls
  if (s->left != NULL) leftr = expr_print_dot(s->left, global_counter);
  if (s->right != NULL) rightr = expr_print_dot(s->right, global_counter);

  //todo transitions

  return local_counter;
}
