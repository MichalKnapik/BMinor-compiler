#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "decl.h"
#include "parser.h"
#include "scope.h"
#include "name_resolution.h"
#include "hash_table.h"
#include "type_check.h"
#include "stack_rbp_pass.h"
#include "codegen.h"
#include "codegen_tools.h"
#include "debug.h"

extern int yyparse();
extern int yylex();
extern decl* program_root;
extern int error_count;
struct hash_table* fundecls = NULL;
struct hash_table* string_store = NULL;

typedef enum {SCAN, PARSE, TYPECHECK, DOT, COMPILE} optchoice;

void print_usage() {
  printf("Usage: bminor (-scan|-typecheck|-dot|-compile) source\n");
  exit(1);
}

optchoice check_options(int argc, char** argv) {
  if (argc != 3) print_usage();
  if (!strcmp(argv[1], "-scan")) return SCAN;
  if (!strcmp(argv[1], "-parse")) return PARSE;  
  if (!strcmp(argv[1], "-typecheck")) return TYPECHECK;  
  if (!strcmp(argv[1], "-dot")) return DOT;
  if (!strcmp(argv[1], "-compile")) return COMPILE;
  print_usage();
  return 0;
}

int main(int argc, char** argv) {

  extern FILE* yyin;
  string_store = hash_table_create(0,0);
  fundecls = hash_table_create(0,0);
  
  optchoice command = check_options(argc, argv);
  if((yyin = fopen(argv[2], "r")) == NULL) {
    perror("Can't read source file. Exiting.\n");
    exit(1);
  }

  if (command == SCAN) {
    int token = 0;
    while ((token = yylex())) {
      if(token == BAD_T) {
	fclose(yyin);    
	printf("Scan failed.\n");
	return 1;
      }
    }
    printf("Scan successful.\n");
    fclose(yyin);    
    return 0;
  }

  if (command == PARSE) {
    if(!yyparse()) {
      printf("Parse successful.\n");
      fclose(yyin);
      return 0;
    } else {
      printf("Parse failed.\n");
      fclose(yyin);
      return 1;
    }
    fclose(yyin);
  }

  if (command == TYPECHECK) {
    if(!yyparse()) {
      if (program_root != NULL) {
	//first pass of typechecking: resolve names and collect string literals
	printf("Name resolution.\n");
	make_scope();
	scope_enter();
	decl_resolve(program_root);
	scope_exit();
	mark_program_symbols_with_rbppos(program_root);
	printf("Found %d error(s) in name resolution.\n", error_count);

	//second pass of typechecking: assign types
	int preverrs = error_count;
	printf("Typechecking.\n");
	decl_typecheck(program_root);
	printf("Found %d error(s) while typechecking.\n", error_count - preverrs);

	hash_table_delete(fundecls);
	hash_table_delete(string_store);
      }
      return error_count > 0;
    } else {
      printf("Parse failed, so no typechecking.\n");
      return 1;
    }    
  }

  if (command == DOT) {
    if(!yyparse()) {
      if (program_root != NULL) {
	make_scope();
	scope_enter();
	decl_resolve(program_root);
	scope_exit();
	mark_program_symbols_with_rbppos(program_root);
	decl_typecheck(program_root);
	if (error_count > 0) printf("Found %d error(s) in name resolution and typechecking and will not produce dot.\n", error_count);
	else print_dot(program_root);
	hash_table_delete(fundecls);
	hash_table_delete(string_store);
      }
    } else {
      printf("parse failed!\n");
      return 1;
    }    
  }

  if (command == COMPILE) {
    if(!yyparse()) {
      if (program_root != NULL) {

	//first pass of typechecking: resolve names and collect string literals
	make_scope();
	scope_enter();
	decl_resolve(program_root);
	scope_exit();
	mark_program_symbols_with_rbppos(program_root);
	if (error_count > 0)
	printf("Found %d error(s) in name resolution.\n", error_count);

	//second pass of typechecking: assign types
	int preverrs = error_count;
	decl_typecheck(program_root);
	if (error_count - preverrs > 0)
	  printf("Found %d error(s) typechecking.\n", error_count - preverrs);

	if (error_count == 0) {//code generation
	  codegen_program_preamble();	  
	  string_store_codegen();
	  codegen_program_externs();	  
	  decl_codegen(program_root);
	  codegen_program_epilogue();
	} else printf("Errors present, can't compile. Bye.\n");
	
	hash_table_delete(fundecls);
	hash_table_delete(string_store);
      }
      return error_count > 0;
    } else {
      printf("parse failed!\n");
      return 1;
    }    
  }

  
  return 0;
}
