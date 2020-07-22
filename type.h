#ifndef TYPE_H
#define TYPE_H

#include "param_list.h"

typedef enum {
	TYPE_VOID,
	TYPE_BOOLEAN,
	TYPE_CHARACTER,
	TYPE_INTEGER,
	TYPE_STRING,
	TYPE_ARRAY,
	TYPE_FUNCTION,
} type_t;

typedef struct type {
	type_t kind;
	struct param_list *params;
	struct type *subtype;
} type;

type* type_create(type_t kind, type *subtype, param_list *params);
int type_print_dot(type *d, int* global_counter);

#endif
