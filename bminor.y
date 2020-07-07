%{
#include <stdio.h>
#include <stdlib.h>

extern int yylineno;
extern char *yytext;
extern int yylex();
extern int yyerror( char *str );
%}

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

%start program
%%

program : STRING_T
 {
 };

%%

int yyerror(char* s) {
    printf("PARSE ERROR in line %d\n", yylineno);
    return 1;
    }
