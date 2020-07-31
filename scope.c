#include "scope.h"
#include <stdio.h>
#include <assert.h>

scope* scope_stack = NULL;
int error_count = 0; /* counts typecheck and re-declaration errors */

void make_scope() {
  if (scope_stack == NULL) scope_stack = malloc(sizeof(scope));
  scope_stack->top = NULL;
  scope_stack->level = 0;
}

void scope_enter() {
  stackentry* currtable = malloc(sizeof(stackentry));
  currtable->table = hash_table_create(0,0);
  currtable->prev = scope_stack->top;
  scope_stack->top = currtable;
  scope_stack->level += 1;
}

void scope_exit() {
  assert(scope_stack != NULL && scope_stack->top != NULL);
  stackentry* prev = scope_stack->top->prev;
  hash_table_delete(scope_stack->top->table);
  free(scope_stack->top);
  scope_stack->top = prev;
  scope_stack->level -= 1;
}

int scope_level() {
  return scope_stack->level;
}

void scope_bind(const char *name, symbol *sym) {
  hash_table_insert(scope_stack->top->table, name, sym);
}

void print_scope() {

  stackentry* currentry = scope_stack->top;
  while (currentry != NULL) {
    char *key = NULL;
    void *value = NULL;

    hash_table_firstkey(currentry->table);
    
    while(hash_table_nextkey(currentry->table, &key, &value)) printf("%s\n", key);
    printf("-----\n");
    currentry = currentry->prev;
  }

}

symbol* scope_lookup(const char *name) {

  stackentry* currentry = scope_stack->top;
  symbol* retval = NULL;
  while (currentry != NULL) {
    retval = hash_table_lookup(currentry->table, name);
    if (retval != NULL) break;
    currentry = currentry->prev;
  }

  return retval;
}

symbol* scope_lookup_current(const char *name) {
  return hash_table_lookup(scope_stack->top->table, name);
}
