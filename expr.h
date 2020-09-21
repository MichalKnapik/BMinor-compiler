#ifndef EXPR_H
#define EXPR_H

#include "symbol.h"

typedef enum {
	EXPR_NAME,

	EXPR_STR,
	EXPR_INT,
	EXPR_CHAR,	
	EXPR_BOOL,

        EXPR_ARR_SUBS,
        EXPR_FUN_CALL,
	
	EXPR_EXP,
	EXPR_MUL,
	EXPR_DIV,
	EXPR_MOD,
	EXPR_ADD,
	EXPR_SUB,

	EXPR_UN_MIN,
	EXPR_INC,
	EXPR_DEC,

	EXPR_LE,
	EXPR_LT,
	EXPR_GT,
	EXPR_GE,

	EXPR_EQ,
	EXPR_NEQ,

	EXPR_AND,
	EXPR_OR,
	EXPR_NEG,

	EXPR_ASSGN,
	EXPR_ARG
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

	symbol *symbol;
        int reg;
} expr;

expr* expr_create(expr_t kind, struct expr *left, expr *right);
expr* expr_create_name(const char *n);
expr* expr_create_integer_literal(int c);
expr* expr_create_boolean_literal(int c);
expr* expr_create_char_literal(char c);
expr* expr_create_string_literal(const char *str);
int expr_print_dot(expr* e, int* global_counter);

int is_expr_int(expr* e);
int is_expr_bool(expr* e);
int is_expr_char(expr* e);
int is_expr_string(expr* e);

#endif
