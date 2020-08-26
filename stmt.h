#ifndef STMT_H
#define STMT_H

typedef struct decl decl;
typedef struct expr expr;

typedef enum {
	STMT_DECL,
	STMT_EXPR,
	STMT_IF_ELSE,
	STMT_FOR,
	STMT_PRINT,
	STMT_RETURN,
	STMT_BLOCK
} stmt_t;

typedef struct stmt {
	stmt_t kind;
	struct decl *decl;
	struct expr *init_expr;
	struct expr *expr;
	struct expr *next_expr;
	struct stmt *body;
	struct stmt *else_body;
	struct stmt *next;
} stmt;


stmt* stmt_create(stmt_t kind, decl *decl, expr *init_expr, expr* exprf, expr *next_expr, stmt *body, stmt *else_body, stmt *next);
int stmt_print_dot(stmt* s, int* global_counter);

#endif
