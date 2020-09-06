#ifndef SYMBOL_H
#define SYMBOL_H

#include "type.h"

typedef enum {
	      SYMBOL_LOCAL,
	      SYMBOL_PARAM,
	      SYMBOL_GLOBAL
} symbol_t;

typedef enum {
	      NO_FLAG,
	      FUN_DEF
} symbol_flags;

typedef struct symbol {
  symbol_t kind;
  symbol_flags flags;
  type *type;
  char *name;

  //multi-purpose field:
  //param number in function, symbol id in names after name resolution,
  //and pointer to stack position of stack variable after stack assignment computation pass
  //and number of string data label if it is a string literal (put in the string store)
  int which;

} symbol;

symbol* symbol_create(symbol_t kind, type *type, char *name);
int is_param_on_stack(symbol* s);
#endif
