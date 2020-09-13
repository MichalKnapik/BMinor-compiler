#include <stdio.h>
#include <stdlib.h>
#include "smalltools.h"
#include "expr.h"
#include "scope.h"
#include "name_resolution.h"
#include "hash_table.h"

extern struct hash_table* string_store; //this is the storage for string literals
int str_ctr = 0;

expr* expr_create(expr_t kind, struct expr *left, expr *right) {

  expr* rval = malloc(sizeof(expr));
  rval->kind = kind;
  rval->left = left;  
  rval->right = right;  

  rval->name = NULL;
  rval->literal_value = 0;
  rval->string_literal = NULL;
  rval->symbol = NULL;
  rval->reg = -1;

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

  if (string_store != NULL) { //collect literals in the string store
    int* string_no = hash_table_lookup(string_store, str);
    if (string_no == NULL) {
      int* str_label = malloc(sizeof(int));
      *str_label = str_ctr++;
      hash_table_insert(string_store, str, str_label);
    } 
  }
  
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
  if (s->kind == EXPR_STR && s->string_literal != NULL) {
    print_with_bar_unless_first(&first, "str = ");        
    printf("%s", s->string_literal);
  }

  if (s->left != NULL) print_with_bar_unless_first(&first, "<f0> left");
  if (s->right != NULL) print_with_bar_unless_first(&first, "<f1> right");

  //printing symbols
  if (s->symbol != NULL && s->kind == EXPR_NAME) {
    print_with_bar_unless_first(&first, "");
    if (s->symbol->which >= 0) {
      if (s->symbol->kind == SYMBOL_LOCAL) printf(" stk ");
      if (s->symbol->kind == SYMBOL_PARAM) printf(" prm ");
      if (s->symbol->kind == SYMBOL_GLOBAL) printf(" glb ");
      else printf("(%d)", s->symbol->which);
    }
    else printf(" global ");
  }

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

int is_expr_int(expr* e) {

  if (e->kind == EXPR_INT || e->kind == EXPR_EXP || e->kind == EXPR_MUL || e->kind == EXPR_DIV ||
      e->kind == EXPR_MOD || e->kind == EXPR_ADD || e->kind == EXPR_SUB || e->kind == EXPR_UN_MIN ||
      e->kind == EXPR_INC || e->kind == EXPR_DEC || (e->kind == EXPR_ASSGN && is_expr_int(e->right))) return 1;

  return 0;
}

int is_expr_bool(expr* e) {
  return (e->kind == EXPR_BOOL || e->kind == EXPR_LE || e->kind == EXPR_LT || e->kind == EXPR_GT ||
	  e->kind == EXPR_GE || e->kind == EXPR_EQ || e->kind == EXPR_NEQ || e->kind == EXPR_AND ||
	  e->kind == EXPR_OR || e->kind == EXPR_NEG || (e->kind == EXPR_ASSGN && is_expr_bool(e->right)));
}

int is_expr_char(expr* e) {
  return (e->kind == EXPR_CHAR || (e->kind == EXPR_ASSGN && is_expr_char(e->right)));
}

int is_expr_string(expr* e) {
  return (e->kind == EXPR_STR || (e->kind == EXPR_ASSGN && is_expr_string(e->right)));
}

