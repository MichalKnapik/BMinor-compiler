%option noyywrap yylineno nounput noinput

%x comment lcomment 

%{
#include <stdio.h>
#include <string.h>
#include "parser.h"
%}

/* reserved words */
ARRAY_L      "array"
BOOLEAN_L    "boolean"
CHAR_L       "char"
ELSE_L       "else"
FALSE_L      "false"
FOR_L        "for"
FUNCTION_L   "function"
IF_L         "if"
INTEGER_L    "integer"
PRINT_L      "print"
RETURN_L     "return"
STRING_L     "string"
TRUE_L       "true"
VOID_L       "void"
WHILE_L      "while"

IDENTIFIER [_a-zA-Z]+[0-9a-zA-Z]*
INTEGER -?[0-9]+
STRING \"([^\\\"\n]|\\[\x00-\x7F]){0,256}\"
CHAR (\'([\x00-\x5B]|[\x5D-\x7F])\')|(\'\\[\x00-\x7F]\')

%%

[ \n\r\t]*		;

"/*"                     BEGIN(comment);
<comment>[^*]*
<comment>"*"+[^/]
<comment><<EOF>>         return BAD_T;    
<comment>"*/"            BEGIN(INITIAL);
"//"                     BEGIN(lcomment);
<lcomment>[^\n]*        
<lcomment>\n             BEGIN(INITIAL);

{ARRAY_L}                return ARRAY_KW_T;
{BOOLEAN_L}              return BOOLEAN_KW_T;
{CHAR_L}                 return CHAR_KW_T;
{ELSE_L}                 return ELSE_KW_T;
{FALSE_L}                return FALSE_KW_T;
{FOR_L}                  return FOR_KW_T;
{FUNCTION_L}             return FUNCTION_KW_T;
{IF_L}                   return IF_KW_T;
{INTEGER_L}              return INTEGER_KW_T;
{PRINT_L}                return PRINT_KW_T;
{RETURN_L}               return RETURN_KW_T;
{STRING_L}               return STRING_KW_T;
{TRUE_L}                 return TRUE_KW_T;
{VOID_L}                 return VOID_KW_T;
{WHILE_L}                return WHILE_KW_T;

"["                      return LBRACKET_T;
"]"                      return RBRACKET_T;
"("                      return LPAREN_T;
")"                      return RPAREN_T;
"{"                      return LCURL_T;
"}"                      return RCURL_T;
","                      return COMMA_T;
":"                      return COLON_T;
";"                      return SEMICOLON_T;

"++"                     return INC_T;
"--"                     return DEC_T;
"!"                      return NEG_T;
"^"                      return EXP_T;
"*"                      return TIMES_T;
"/"                      return DIV_T;
"%"                      return MOD_T;
"+"                      return PLUS_T;
"-"                      return MINUS_T;

"<"                      return LT_T;
"<="                     return LE_T;
">"                      return GT_T;
">="                     return GE_T;
"=="                     return EQ_T;
"!="                     return NEQ_T;
"&&"                     return AND_T;
"||"                     return OR_T;
"="                      return ASSG_T;

{IDENTIFIER}             return IDENTIFIER_T;
{INTEGER}                return INTEGER_T;
{STRING}                 return STRING_T;
{CHAR}                   return CHAR_T;

.			 return BAD_T;
%%
