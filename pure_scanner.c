#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <inttypes.h>
#include "parser.h"

extern FILE *yyin;
extern char *yytext;

int main(int argc, char** argv) {

  if(argc > 1) {
    if(!(yyin = fopen(argv[2], "r"))) {
      perror(argv[2]);
      return (1);
    }
  }


  enum yytokentype res = BAD_T;
  while ((res = yylex())) {
    printf("%s\n", yytext);
    if (res == BAD_T) return 1;
  }
  return 0;
}
