#include <stdio.h>
#include <stdlib.h>
#include "decl.h"

decl* decl_create(char *name, type *type, expr *value, stmt *code, decl *next) {

  decl* rval = malloc(sizeof(decl));
  rval->name = name;
  rval->type = type;
  rval->value = value;
  rval->code = code;
  rval->next = next;
  
  return rval;
}
