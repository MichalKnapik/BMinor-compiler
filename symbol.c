#include <stdlib.h>
#include "symbol.h"

struct symbol* symbol_create(symbol_t kind, struct type *type, char *name) {

  symbol* rval = malloc(sizeof(symbol));
  rval->kind = kind;
  rval->type = type;
  rval->name = name;
  rval->which = -1;
  rval->flags = NO_FLAG;

  return rval;
}


int is_param_on_stack(symbol* s) {
  return s->which > 5;
}
