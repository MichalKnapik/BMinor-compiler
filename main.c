#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "decl.h"
#include "parser.h"
#include "scope.h"

extern int yyparse();
extern int yylex();
extern decl* program_root;

int main(int argc, char** argv) {

  extern FILE* yyin;

  if (!strcmp(argv[1], "-scan")) {
     
    if((yyin = fopen(argv[2], "r")) == NULL) {
      perror("an issue with reading models");
      exit(1);
    }

    int token = 0;
    while ((token = yylex())) {
      if(token == BAD_T) {
	printf("scan failed\n");
	return 1;
      }
    }

    printf("scan successful\n");
    fclose(yyin);    

    return 0;
  }

  if (!strcmp(argv[1], "-parse")) {
    if((yyin = fopen(argv[2], "r")) == NULL) {
      perror("an issue with reading models");
      exit(1);
    }

    if(!yyparse()) {
      int global_counter = 0;
      if (program_root != NULL) {
	printf("digraph{\nnode [shape=record];\n");
	decl_print_dot(program_root, &global_counter);
	printf("}\n");
      }

      return 0;
    } else {
      printf("parse failed!\n");
      return 1;
    }

    fclose(yyin);
  }

  if (!strcmp(argv[1], "-typecheck")) {
    if((yyin = fopen(argv[2], "r")) == NULL) {
      perror("an issue with reading models");
      exit(1);
    }
    if(!yyparse()) {
      //todo now
      if (program_root != NULL) {
	make_scope();
	scope_enter();
	decl_resolve(program_root);
	scope_exit();
      }
      return 0;
    } else {
      printf("parse failed!\n");
      return 1;
    }    
  }
  
  return 0;
}
