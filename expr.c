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
