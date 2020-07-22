#ifndef EXPR_H
#define EXPR_H

#include "symbol.h"

typedef enum {
	EXPR_NAME,
	EXPR_STR,
	EXPR_INT,
	EXPR_CHAR,	
	EXPR_BOOL,
	EXPR_ARG,
        EXPR_ARR_SUBS,
        EXPR_FUN_CALL,
	EXPR_UN_MIN,
	EXPR_NEG,
	EXPR_EXP,
	EXPR_MUL,
	EXPR_DIV,
	EXPR_MOD,
	EXPR_ADD,
	EXPR_SUB,
	EXPR_INC,
	EXPR_DEC,
	EXPR_LE,
	EXPR_LT,
	EXPR_EQ,
	EXPR_GT,
	EXPR_GE,
	EXPR_NEQ,
	EXPR_AND,
	EXPR_OR,
	EXPR_ASSGN	
} expr_t;

typedef struct expr {

	/* used by all kinds of exprs */
 	expr_t kind;
	struct expr *left;
	struct expr *right;

	/* used by various leaf exprs */
	const char *name;
	int literal_value;
	const char * string_literal;
	struct symbol *symbol;

} expr;

expr* expr_create(expr_t kind, struct expr *left, expr *right);
expr* expr_create_name(const char *n);
expr* expr_create_integer_literal(int c);
expr* expr_create_boolean_literal(int c);
expr* expr_create_char_literal(char c);
expr* expr_create_string_literal(const char *str);
int expr_print_dot(expr* s, int* global_counter);

#endif
