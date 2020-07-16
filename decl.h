#ifndef DECL_H
#define DECL_H

#include "type.h"
#include "stmt.h"
#include "expr.h"

typedef struct decl {
	char *name;
	struct type *type;
	struct expr *value;
	struct stmt *code;
	struct symbol *symbol;
	struct decl *next;
} decl;

decl* decl_create(char *name, type *type, expr *value, stmt *code, decl *next);

void decl_print(decl *d, int indent); //TODO

#endif


