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
        int which; //param number in function, symbol id in names
} symbol;

symbol* symbol_create(symbol_t kind, type *type, char *name);

#endif
