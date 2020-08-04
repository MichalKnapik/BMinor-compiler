#include <stdio.h>
#include <stdlib.h>
#include "smalltools.h"
#include "expr.h"
#include "scope.h"

extern int error_count;

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

  const char * exprnames [] = {"EXPR_NAME", "EXPR_STR", "EXPR_INT", "EXPR_CHAR", "EXPR_BOOL", "EXPR_ARR_SUBS",
			       "EXPR_FUN_CALL", "EXPR_EXP", "EXPR_MUL", "EXPR_DIV", "EXPR_MOD", "EXPR_ADD",
			       "EXPR_SUB", "EXPR_UN_MIN", "EXPR_INC", "EXPR_DEC", "EXPR_LE", "EXPR_LT", "EXPR_GT",
			       "EXPR_GE", "EXPR_EQ", "EXPR_NEQ", "EXPR_AND", "EXPR_OR", "EXPR_NEG", "EXPR_ASSGN",
			       "EXPR_ARG"};

  const char* kindr = exprnames[s->kind - EXPR_NAME];

  printf("struct%d [label=\"", local_counter);
  printf("{%s|{", kindr);

  int first = 0;
  if (s->name != NULL) {
    print_with_bar_unless_first(&first, "nme = ");
    printf("%s", s->name);
  }
  if (s->kind == EXPR_INT || s->kind == EXPR_CHAR || s->kind == EXPR_BOOL) {
    print_with_bar_unless_first(&first, "lvl = ");    
    printf("%d", s->literal_value);
  }
  if (s->string_literal != NULL) {
    print_with_bar_unless_first(&first, "str = ");        
    printf("%s", s->string_literal);
  }
  if (s->left != NULL) print_with_bar_unless_first(&first, "<f0> left");
  if (s->right != NULL) print_with_bar_unless_first(&first, "<f1> right");

  printf("}}\"];\n");

  //rec calls
  if (s->left != NULL) {
    leftr = expr_print_dot(s->left, global_counter);
    printf("struct%d:f0 -> struct%d;\n", local_counter, leftr);    
  }
  if (s->right != NULL) {
    rightr = expr_print_dot(s->right, global_counter);
    printf("struct%d:f1 -> struct%d;\n", local_counter, rightr);        
  }

  return local_counter;
}

void expr_resolve(expr *e) {

  if (e == NULL) return;

  if(e->kind == EXPR_NAME) {
    e->symbol = scope_lookup(e->name);
    if (e->symbol == NULL) {
      printf("Error in name resolution: unknown name %s.\n", e->name);
      error_count++;
    }

  } else {
    expr_resolve(e->left);
    expr_resolve(e->right);
  }

}
