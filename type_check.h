#ifndef TYPE_CHECK_H
#define TYPE_CHECK_H
#include "type.h"
#include "expr.h"
#include "decl.h"
#include "stmt.h"

type* expr_typecheck(expr *e);
void decl_typecheck(decl *d);
void array_typecheck(expr *e, type* itemtype, const char* arrname);
void stmt_typecheck(stmt *s);

#endif
