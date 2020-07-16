#include <stdio.h>
#include <stdlib.h>
#include "param_list.h"
#include "type.h"

param_list* param_list_create(char *name, type *type, param_list *next) {

  param_list* rval = malloc(sizeof(param_list));
  rval->name = name;
  rval->type = type;
  rval->next = next;
  rval->symbol = NULL;

  return rval;
}
