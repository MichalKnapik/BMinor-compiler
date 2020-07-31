#ifndef SCOPE_H
#define SCOPE_H

#include <stdlib.h>
#include "symbol.h"
#include "hash_table.h"

struct stackentry_t {
  struct hash_table* table;
  struct stackentry_t* prev;
};
typedef struct stackentry_t stackentry;

struct scope_t {
  stackentry* top;
  int level;
};
typedef struct scope_t scope;

void make_scope();
void scope_enter();
void scope_exit();
int scope_level();
void scope_bind(const char *name, symbol *sym);
void print_scope();

symbol* scope_lookup(const char *name);
symbol* scope_lookup_current(const char *name);


#endif
