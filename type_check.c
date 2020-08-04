#include <stdlib.h>
#include <stdio.h>
#include "type_check.h"
#include "type.h"
#include "stmt.h"
#include "param_list.h"

extern int error_count;

int is_basic(type* a) {
  return (a->kind == TYPE_BOOLEAN || a->kind == TYPE_CHARACTER || a->kind == TYPE_INTEGER || a->kind == TYPE_STRING);
}

int type_equals(type *a, type *b) {

  if (a->kind == b->kind) {
    if (is_basic(a)) return 1;
    if (a->kind == TYPE_ARRAY) return type_equals(a->subtype, b->subtype);
    if (a->kind == TYPE_FUNCTION) {

      param_list* pla = a->params;
      param_list* plb = b->params;

      while (1) {
	if (pla == NULL && plb == NULL) break;
	else if ((pla != NULL && plb == NULL) || (pla == NULL && plb != NULL)) return 0;
	if (!type_equals(pla->type, plb->type)) return 0;

	pla = pla->next;
	plb = plb->next;	
      }

      return type_equals(a->subtype, b->subtype);
    }

  }

  return 0;
}

type* type_copy(type *t) {
  return type_create(t->kind, type_copy(t->subtype), param_list_copy(t->params));
}

void type_delete(type *t) {
  param_list_delete(t->params);
  type_delete(t->subtype);
  free(t);
}

type* expr_typecheck(expr *e) {

  if(e == NULL) return NULL;
  type *lt = expr_typecheck(e->left);
  type *rt = expr_typecheck(e->right);

  type *result = NULL;
  switch(e->kind) {

  case EXPR_NAME:
    result = type_copy(e->symbol->type);
    break;

  case EXPR_STR:
    result = type_create(TYPE_STRING, NULL, NULL);
    break;
  case EXPR_INT:
    result = type_create(TYPE_INTEGER, NULL, NULL);
    break;
  case EXPR_CHAR:
    result = type_create(TYPE_CHARACTER, NULL, NULL);
    break;
  case EXPR_BOOL:
    result = type_create(TYPE_BOOLEAN, NULL, NULL);
    break;

  case EXPR_ARR_SUBS:
    if (lt->kind == TYPE_ARRAY) {
      if (rt->kind != TYPE_INTEGER) {
	printf("Error: non-integer index used when accessing array %s.\n", e->left->name);
	++error_count;
      }
      result = type_copy(lt->subtype);
    } else {
	printf("Error: %s is not an array.\n", e->left->name);
	++error_count;
      result = type_copy(lt);
    }
    break;    

  case EXPR_FUN_CALL:

    result = type_copy(lt);
    const char* fname = NULL;
    if (e->left != NULL && e->left->name != NULL) fname = e->left->name;

    if (result->kind != TYPE_FUNCTION) {
	printf("Error: unknown function %s.\n", fname);
	++error_count;
	break;
    }
    
    expr* arg = e->right;
    type* argtype = NULL;
    type* partype = NULL;    
    param_list* declared_param = lt->params;
    while (arg != NULL) {

      if (declared_param == NULL) {
	printf("Error: too many arguments in call to %s.\n", fname);
	++error_count;
	break;	
      }

      argtype = expr_typecheck(arg);
      partype = declared_param->type;

      if (!type_equals(argtype, partype)) {
	printf("Error: parameter type mismatch in call to %s.\n", fname);
	++error_count;	
      }

      type_delete(argtype);
      
      arg = arg->right;
      declared_param = declared_param->next;
    }

    if (declared_param != NULL) {
      printf("Error: too few arguments in call to %s.\n", fname);
      ++error_count;
    }
    break;    

  case EXPR_EXP:
  case EXPR_MUL:
  case EXPR_DIV:
  case EXPR_MOD:        
  case EXPR_ADD:
  case EXPR_SUB:
    if (lt->kind != TYPE_INTEGER || rt->kind != TYPE_INTEGER) {
      printf("Error: arithmetic operation on non-integers.\n");
      ++error_count;
    }
    result = type_create(TYPE_INTEGER, NULL, NULL);
    break;
  case EXPR_UN_MIN:
  case EXPR_INC:
  case EXPR_DEC:
    if (lt->kind != TYPE_INTEGER) {
      printf("Error: (unary) arithmetic operation on non-integers.\n");
      ++error_count;      
    }
    result = type_create(TYPE_INTEGER, NULL, NULL);
    break;

  case EXPR_LE:
  case EXPR_LT:
  case EXPR_GT:
  case EXPR_GE:
    if (lt->kind != TYPE_INTEGER || rt->kind != TYPE_INTEGER) {
      printf("Error: arithmetic comparisons on non-integers.\n");
      ++error_count;
    }
    result = type_create(TYPE_BOOLEAN, NULL, NULL);
    break;

  case EXPR_EQ:
  case EXPR_NEQ:
    if (!type_equals(lt,rt)) {
      printf("Error: comparing objects of different types.\n");
      ++error_count;      
    }
    if (lt->kind==TYPE_VOID || lt->kind==TYPE_ARRAY || lt->kind==TYPE_FUNCTION) {
      printf("Error: void/array/function type in comparison.\n");
      ++error_count;            
    }
    result = type_create(TYPE_BOOLEAN, NULL, NULL);
    break;

  case EXPR_AND:
  case EXPR_OR:
    if (lt->kind != TYPE_BOOLEAN || rt->kind != TYPE_BOOLEAN) {
      printf("Error: boolean operation on non-bools.\n");
      ++error_count;      
    }
    result = type_create(TYPE_BOOLEAN, NULL, NULL);
    break;
  case EXPR_NEG:
    if (lt->kind != TYPE_BOOLEAN) {
      printf("Error: (unary) boolean operation on non-bools.\n");
      ++error_count;      
    }
    result = type_create(TYPE_BOOLEAN, NULL, NULL);
    break;

  case EXPR_ARG:
    result = type_copy(lt);
    break;

  case EXPR_ASSGN:

    result = type_copy(lt);
    if (e->left->kind != EXPR_NAME) {
      printf("Error: assignment to non-variable.\n");
      ++error_count;
    } else {
      const char* varname = e->left->name;
      //detecting arrays in assignemnts, not pretty, should be done in AST
      if (e->right->kind == EXPR_ARG) { 
	printf("Error: can't assign array to %s.\n", varname);
	++error_count;
	break;
      }

      if (!type_equals(lt,rt)) {
	printf("Error: assigning object of wrong type to %s.\n", varname);
	++error_count;      
      }

    }
    break;
  }

  type_delete(lt);
  type_delete(rt);

  return result;
}

void decl_typecheck(decl *d) {

  if (d->value != NULL) {//typecheck var decl

    //typecheck array (again - it'd be better to deal with array detection in AST)
    if (d->value->kind == EXPR_ARG) { 
      if (d->type->kind == TYPE_ARRAY) { 
	array_typecheck(d->value, d->type, d->name);
      } else {
	printf("Error: can't assign array to %s.\n", d->name);
	++error_count;
      }
      return;
    }

    //typecheck vars
    if (d->type->kind == TYPE_ARRAY) {
      printf("Error: can't assign non-array init value to %s.\n", d->name);
      ++error_count;
      return;
    }

    type *t = expr_typecheck(d->value);

    if(!type_equals(t, d->symbol->type)) {
      printf("Error: mismatch in initialisation of var %s.\n", d->name);
      ++error_count;
    }

    type_delete(t);
  }

  if(d->code) stmt_typecheck(d->code);
}

void array_typecheck(expr *e, type* itemtype, const char* arrname) {

  while (e != NULL) {

    if (e->kind != EXPR_ARG) {
      printf("Error: %s is not an array.\n", arrname);
      ++error_count;
      return;
    }

    type* curritemtype = expr_typecheck(e->left);
    if (!type_equals(itemtype, curritemtype)) {
      printf("Error: array %s type init mismatch.\n", arrname);
      ++error_count;
    }
    e = e->right;
    type_delete(itemtype);
  }

}


void stmt_typecheck(stmt *s) {

  //curr todo

  /* type *t; */
  /* switch(s->kind) { */
  /* case STMT_EXPR: */
  /*   t = expr_typecheck(s->expr); */
  /*   type_delete(t); */
  /*   break; */
  /* case STMT_BLOCK: */
  /*   t = expr_typecheck(s->body); */
  /*   type_delete(t); */
  /*   break; */
  /* case STMT_IF_THEN: */
  /*   t = expr_typecheck(s->expr); */
  /*   if(t->kind!=TYPE_BOOLEAN) { */
  /*     /\* display an error *\/ */
  /*   } */
  /*   type_delete(t); */
  /*   stmt_typecheck(s->body); */
  /*   stmt_typecheck(s->else_body); */
  /*   break; */
  /*   /\* more cases here *\/ */
  /* } */
}
