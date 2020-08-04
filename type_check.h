#ifndef TYPE_CHECK_H
#define TYPE_CHECK_H
#include "type.h"
#include "expr.h"
#include "decl.h"
#include "stmt.h"

int is_basic(type* a);
int type_equals(type *a, type *b);
type* type_copy(type *t);
void type_delete(type *t);

//todo - below
type* expr_typecheck(expr *e);
void decl_typecheck(decl *d);
void array_typecheck(expr *e, type* itemtype, const char* arrname);
void stmt_typecheck(stmt *s);

#endif
