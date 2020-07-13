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
};

declarations: declaration declarations
{
  //todo*
}
| declaration
{
  //todo*
};

declaration: IDENTIFIER_T COLON_T FUNCTION_KW_T returntype LPAREN_T paramlist RPAREN_T optfbody 
{
  //todo*
}
| IDENTIFIER_T COLON_T type optinit SEMICOLON_T
{
  //todo*
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
{
  //todo*  
};

optinit:
{
  //standard init val?
  //or nothing?
  //todo*
}
| ASSG_T expression
{
  //todo*
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
  //todo*
}
| statement
{
  //todo*
};

statement: IDENTIFIER_T COLON_T type optinit SEMICOLON_T
{
  //declaration
  //todo* 
}
| expression SEMICOLON_T
{
  //todo*
}
| IF_KW_T LPAREN_T expression RPAREN_T statement %prec LOWER_THAN_ELSE
{
  //todo*  
}
| IF_KW_T LPAREN_T expression RPAREN_T statement ELSE_KW_T statement
{
  //todo*  
}
| FOR_KW_T LPAREN_T optexpression SEMICOLON_T optexpression SEMICOLON_T optexpression RPAREN_T statement
{
  //todo*  
}
| PRINT_KW_T SEMICOLON_T
{
  //todo*
}
| PRINT_KW_T exprlist SEMICOLON_T
{
  //todo*
}
| RETURN_KW_T expression SEMICOLON_T
{
  //todo*
}
| LCURL_T statements RCURL_T
{
  //todo*  
};

paramlist:
{
  //todo*
}
| noneptyparamlist
{
  //todo*  
}; 

noneptyparamlist: param 
{
  //todo*
}
| param COMMA_T noneptyparamlist
{
  //todo*
};

param: IDENTIFIER_T COLON_T type 
{
  //todo*
};

expression: LPAREN_T expression RPAREN_T
{
  //todo*
}
| TRUE_KW_T
{
  //todo*
}
| FALSE_KW_T
{
  //todo*
}
| CHAR_T
{
  //todo*
}
| INTEGER_T
{
  //todo*
}
| IDENTIFIER_T
{
  //todo*
}
| STRING_T
{
  //todo*
}
| LCURL_T arrayelementlist RCURL_T 
{
  //todo*
}  
| IDENTIFIER_T arrindexselect
{
  //todo*
}
| IDENTIFIER_T LPAREN_T exprlist RPAREN_T
{
  //todo*
}
| IDENTIFIER_T INC_T
{
  //todo*
}
| IDENTIFIER_T DEC_T
{
  //todo*
}
| MINUS_T expression %prec UNARYOP
{
  //todo*
}
| NEG_T expression %prec UNARYOP
{
  //todo*
}
| expression EXP_T expression
{
  //todo*
}
| expression TIMES_T expression
{
  //todo*
}
| expression DIV_T expression
{
  //todo*
}
| expression MOD_T expression
{
  //todo*
}
| expression PLUS_T expression
{
  //todo*
}
| expression MINUS_T expression
{
  //todo*
}
| expression LT_T expression
{
  //todo*
}
| expression LE_T expression
{
  //todo*
}
| expression GT_T expression
{
  //todo*
}
| expression GE_T expression
{
  //todo*
}
| expression EQ_T expression
{
  //todo*
}
| expression NEQ_T expression
{
  //todo*
}
| expression AND_T expression
{
  //todo*
}
| expression OR_T expression
{
  //todo*
}
| expression ASSG_T expression
{
  //todo*
};

arrindexselect: LBRACKET_T expression RBRACKET_T
{
  //todo*
}
| LBRACKET_T expression RBRACKET_T arrindexselect
{
  //todo*
};

optexpression:
{
  //todo*
}
| expression
{
  //todo*
};

arrayelementlist: expression
{
  //todo*
}
| expression COMMA_T arrayelementlist
{
  //todo*
}

exprlist: expression
{
  //todo*
}
| expression COMMA_T exprlist
{
  //todo*
}

%%

int yyerror(char* s) {
    printf("PARSE ERROR in line %d\n", yylineno);
    return 1;
    }
