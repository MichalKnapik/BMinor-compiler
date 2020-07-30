#ifndef SYMBOL_H
#define SYMBOL_H

#include "type.h"

typedef enum {
	SYMBOL_LOCAL,
	SYMBOL_PARAM,
	SYMBOL_GLOBAL
} symbol_t;

typedef struct symbol {
	symbol_t kind;
	type *type;
	char *name;
	int which;
} symbol;

symbol* symbol_create(symbol_t kind, type *type, char *name);

#endif
