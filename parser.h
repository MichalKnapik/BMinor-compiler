/* A Bison parser, made by GNU Bison 3.4.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2019 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_PARSER_H_INCLUDED
# define YY_YY_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    BAD_T = 258,
    IDENTIFIER_T = 259,
    INTEGER_T = 260,
    STRING_T = 261,
    CHAR_T = 262,
    ARRAY_KW_T = 263,
    BOOLEAN_KW_T = 264,
    CHAR_KW_T = 265,
    ELSE_KW_T = 266,
    FALSE_KW_T = 267,
    FOR_KW_T = 268,
    FUNCTION_KW_T = 269,
    IF_KW_T = 270,
    INTEGER_KW_T = 271,
    PRINT_KW_T = 272,
    RETURN_KW_T = 273,
    STRING_KW_T = 274,
    TRUE_KW_T = 275,
    VOID_KW_T = 276,
    WHILE_KW_T = 277,
    LBRACKET_T = 278,
    RBRACKET_T = 279,
    LPAREN_T = 280,
    RPAREN_T = 281,
    LCURL_T = 282,
    RCURL_T = 283,
    COMMA_T = 284,
    COLON_T = 285,
    SEMICOLON_T = 286,
    INC_T = 287,
    DEC_T = 288,
    NEG_T = 289,
    EXP_T = 290,
    TIMES_T = 291,
    DIV_T = 292,
    MOD_T = 293,
    PLUS_T = 294,
    MINUS_T = 295,
    LT_T = 296,
    LE_T = 297,
    GT_T = 298,
    GE_T = 299,
    EQ_T = 300,
    NEQ_T = 301,
    AND_T = 302,
    OR_T = 303,
    ASSG_T = 304,
    UNARYOP = 305,
    LOWER_THAN_ELSE = 306
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 15 "bminor.y"

  /* lexer token types */
  char* stringval;
  int intval;  
  char charval;

  /* parser (non)terminal types */
  decl* decl_t;
  expr* expr_t;
  param_list* param_l_t;
  stmt* stmt_t;

#line 122 "parser.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_PARSER_H_INCLUDED  */
