%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "decl.h"
#include "stmt.h"
#include "param_list.h"

extern int yylineno;
extern char *yytext;
extern int yylex();
extern int yyerror(char *str);
%}

%union {
  /* lexer token types */
  char* stringval;
  int intval;  
  char charval;

  /* parser (non)terminal types */
  decl* decl_t;
  expr* expr_t;
  param_list* param_l_t;
  stmt* stmt_t;
}

%token BAD_T
%token IDENTIFIER_T
%token INTEGER_T
%token STRING_T
%token CHAR_T

%token ARRAY_KW_T
%token BOOLEAN_KW_T
%token CHAR_KW_T
%token ELSE_KW_T
%token FALSE_KW_T
%token FOR_KW_T
%token FUNCTION_KW_T
%token IF_KW_T
%token INTEGER_KW_T
%token PRINT_KW_T
%token RETURN_KW_T
%token STRING_KW_T
%token TRUE_KW_T
%token VOID_KW_T
%token WHILE_KW_T

%token LBRACKET_T;
%token RBRACKET_T;
%token LPAREN_T;
%token RPAREN_T;
%token LCURL_T;
%token RCURL_T;
%token COMMA_T;
%token COLON_T;
%token SEMICOLON_T;

%token INC_T;
%token DEC_T;
%token NEG_T;
%token EXP_T;
%token TIMES_T;
%token DIV_T;
%token MOD_T;
%token PLUS_T;
%token MINUS_T;

%token LT_T;
%token LE_T;
%token GT_T;
%token GE_T;
%token EQ_T;
%token NEQ_T;
%token AND_T;
%token OR_T;
%token ASSG_T;

%type <charval> CHAR_T
%type <intval> INTEGER_T
%type <stringval> STRING_T IDENTIFIER_T
%type <decl_t> declaration declarations
%type <expr_t> expression optexpression arrayelementlist arrindexselect exprlist exprlist_n optinit
%type <stmt_t> statement statements

%left INC_T DEC_T
%right UNARYOP
%right EXP_T
%left TIMES_T DIV_T MOD_T
%left PLUS_T MINUS_T
%left LT_T LE_T GT_T GE_T EQ_T NEQ_T
%left AND_T OR_T
%left ASSG_T

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE_KW_T

%start program
%%

program:
{
  //todo*  
}
| declarations
{
  //todo*
  decl* d = $1;
  while (d != NULL) {
    printf("(%s)\n", d->name);
    d = d->next;
  }
};

declarations: declaration declarations
{
  $1->next = $2;
  $$ = $1;
}
| declaration
;

declaration: IDENTIFIER_T COLON_T FUNCTION_KW_T returntype LPAREN_T paramlist RPAREN_T optfbody 
{
  //todonow 2
  decl* dc = decl_create($1, NULL, NULL, NULL, NULL);
  $$ = dc;
}
| IDENTIFIER_T COLON_T type optinit SEMICOLON_T
{
  //todonow 1
  decl* dc = decl_create(strdup($1), NULL, NULL, NULL, NULL);
  $$ = dc;  
};

returntype: CHAR_KW_T | BOOLEAN_KW_T | INTEGER_KW_T | STRING_KW_T | VOID_KW_T
{ 
 //todo*
};

type: INTEGER_KW_T | BOOLEAN_KW_T | STRING_KW_T | CHAR_KW_T | ARRAY_KW_T LBRACKET_T arrsize RBRACKET_T type
{
  //todo*
};

arrsize:
{
  //todo*  
}
| INTEGER_T
;

optinit:
{
  $$ = NULL;
}
| ASSG_T expression
{
  $$ = $2;
};

optfbody: SEMICOLON_T
{
  //standard init val?
  //or nothing?
  //todo*
}
| ASSG_T LCURL_T statements RCURL_T
{
  //todo*
};

statements: statement statements
{
  $1->next = $2;
}
| statement;

statement: IDENTIFIER_T COLON_T type optinit SEMICOLON_T
{
  //todo - add type of declared symbol in decl_create
  $$ = stmt_create(STMT_DECL,
		   decl_create($1, NULL, $4, NULL, NULL),
		   NULL, NULL, NULL, NULL, NULL, NULL);
}
| expression SEMICOLON_T
{
  $$ = stmt_create(STMT_EXPR, NULL, NULL, $1, NULL, NULL, NULL, NULL);
}
| IF_KW_T LPAREN_T expression RPAREN_T statement %prec LOWER_THAN_ELSE
{
  $$ = stmt_create(STMT_IF_ELSE, NULL, NULL, $3, NULL, $5, NULL, NULL);
}
| IF_KW_T LPAREN_T expression RPAREN_T statement ELSE_KW_T statement
{
  $$ = stmt_create(STMT_IF_ELSE, NULL, NULL, $3, NULL, $5, $7, NULL);
}
| FOR_KW_T LPAREN_T optexpression SEMICOLON_T optexpression SEMICOLON_T optexpression RPAREN_T statement
{
  $$ = stmt_create(STMT_FOR, NULL,
		   $3,
		   $5,
		   $7,
		   $9, NULL, NULL);
}
| PRINT_KW_T exprlist SEMICOLON_T
{
  $$ = stmt_create(STMT_PRINT, NULL, NULL, $2, NULL, NULL, NULL, NULL);
}
| RETURN_KW_T expression SEMICOLON_T
{
  $$ = stmt_create(STMT_RETURN, NULL, NULL, $2, NULL, NULL, NULL, NULL);
}
| LCURL_T statements RCURL_T
{
  $$ = stmt_create(STMT_BLOCK, NULL, NULL, NULL, NULL, $2, NULL, NULL);
};

paramlist:
{
  //todo*
}
| nonemptyparamlist
{
  //todo*  
}; 

nonemptyparamlist: param 
{
  //todo*
}
| param COMMA_T nonemptyparamlist
{
  //todo*
};

param: IDENTIFIER_T COLON_T type 
{
  //todo*
};

expression: LPAREN_T expression RPAREN_T
{
  $$ = $2;
}
| TRUE_KW_T
{
  $$ = expr_create_boolean_literal(1);
}
| FALSE_KW_T
{
  $$ = expr_create_boolean_literal(0);
}
| CHAR_T
{
  $$ = expr_create_char_literal($1);
}
| INTEGER_T
{
  $$ = expr_create_integer_literal($1);
}
| IDENTIFIER_T
{
  $$ = expr_create_name($1);
}
| STRING_T
{
  $$ = expr_create_string_literal($1);
}
| LCURL_T arrayelementlist RCURL_T 
{
  $$ = $2;
}  
| IDENTIFIER_T arrindexselect
{
  $$ = expr_create(EXPR_ARR_SUBS,
		   expr_create_name($1),
		   $2);
}
| IDENTIFIER_T LPAREN_T exprlist RPAREN_T
{
  $$ = expr_create(EXPR_FUN_CALL,
		   expr_create_name($1),
		   $3);
}
| IDENTIFIER_T INC_T
{
  $$ = expr_create(EXPR_INC, expr_create_name($1), NULL);
}
| IDENTIFIER_T DEC_T
{
  $$ = expr_create(EXPR_DEC, expr_create_name($1), NULL);
}
| MINUS_T expression %prec UNARYOP
{
  $$ = expr_create(EXPR_UN_MIN, $2, NULL);
}
| NEG_T expression %prec UNARYOP
{
  $$ = expr_create(EXPR_NEG, $2, NULL);
}
| expression EXP_T expression
{
  $$ = expr_create(EXPR_EXP, $1, $3);
}
| expression TIMES_T expression
{
  $$ = expr_create(EXPR_MUL, $1, $3);
}
| expression DIV_T expression
{
  $$ = expr_create(EXPR_DIV, $1, $3);
}
| expression MOD_T expression
{
  $$ = expr_create(EXPR_MOD, $1, $3);
}
| expression PLUS_T expression
{
  $$ = expr_create(EXPR_ADD, $1, $3);
}
| expression MINUS_T expression
{
  $$ = expr_create(EXPR_SUB, $1, $3);
}
| expression LT_T expression
{
  $$ = expr_create(EXPR_LT, $1, $3);
}
| expression LE_T expression
{
  $$ = expr_create(EXPR_LE, $1, $3);
}
| expression GT_T expression
{
  $$ = expr_create(EXPR_GT, $1, $3);
}
| expression GE_T expression
{
  $$ = expr_create(EXPR_GE, $1, $3);
}
| expression EQ_T expression
{
  $$ = expr_create(EXPR_EQ, $1, $3);
}
| expression NEQ_T expression
{
  $$ = expr_create(EXPR_NEQ, $1, $3);
}
| expression AND_T expression
{
  $$ = expr_create(EXPR_AND, $1, $3);
}
| expression OR_T expression
{
  $$ = expr_create(EXPR_OR, $1, $3);
}
| expression ASSG_T expression
{
  $$ = expr_create(EXPR_ASSGN, $1, $3);  
};

arrindexselect: LBRACKET_T expression RBRACKET_T
{
  $$ = expr_create(EXPR_ARG, $2, NULL);
}
| LBRACKET_T expression RBRACKET_T arrindexselect
{
  $$ = expr_create(EXPR_ARG, $2, $4);
};

optexpression:
{
  $$ = NULL;
}
| expression;

arrayelementlist: expression
{
  $$ = expr_create(EXPR_ARG, $1, NULL);
}
| expression COMMA_T arrayelementlist
{
  $$ = expr_create(EXPR_ARG, $1, $3);
}

exprlist: 
{
  $$ = NULL;
}
| exprlist_n
{
  $$ = $1;
};

exprlist_n:
expression
{
  $$ = expr_create(EXPR_ARG, $1, NULL);
}
| expression COMMA_T exprlist_n
{
  $$ = expr_create(EXPR_ARG, $1, $3);
}

%%

int yyerror(char* s) {
    printf("PARSE ERROR in line %d.\n", yylineno);
    return 1;
    }
