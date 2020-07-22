#ifndef PARAM_LIST_H
#define PARAM_LIST_H

typedef struct type type;

typedef struct param_list {
	char *name;
	struct type *type;
	struct symbol *symbol;
	struct param_list *next;
} param_list;

param_list* param_list_create(char *name, type *type, param_list *next);
int param_print_dot(param_list* l, int* global_counter);

#endif
