#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "decl.h"
#include "parser.h"
#include "scope.h"
#include "name_resolution.h"
#include "hash_table.h"
#include "type_check.h"
#include "stack_rbp_pass.h"
#include "codegen.h"
#include "debug.h"

extern int yyparse();
extern int yylex();
extern decl* program_root;
extern int error_count;
struct hash_table* fundecls = NULL;
struct hash_table* string_store = NULL;

//CLEAN ME UP, BEFORE YOU GO, GO

int main(int argc, char** argv) {

  extern FILE* yyin;
  string_store = hash_table_create(0,0);

  fundecls = hash_table_create(0,0);

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
      if (program_root != NULL) print_dot(program_root);
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

	//first pass of typechecking: resolve names and collect string literals
	printf("Name resolution...\n");
	make_scope();
	scope_enter();
	decl_resolve(program_root);
	scope_exit();
	mark_program_symbols_with_rbppos(program_root);
	//mark_program_symbols_with_numbers(program_root);
	printf("Found %d error(s) in name resolution.\n", error_count);

	//second pass of typechecking: assign types
	int preverrs = error_count;
	printf("Typechecking...\n");
	decl_typecheck(program_root);
	printf("Found %d error(s) while typechecking.\n", error_count - preverrs);

	//print dot
	//	print_dot(program_root);
	//generate code here

	string_store_codegen();
	decl_codegen(program_root);
	/* printf("\n"); */
	/* expr_codegen(code->next->expr); */
	/* printf("\n");	 */
	/* expr_codegen(code->next->next->expr); */
	/* printf("\n");	 */
	//	printf("epilogue\n");	
	//	function_epilogue_codegen(program_root->next->next); //test	
	//	print_mem_pos_decl(program_root);

	//a bit of storage cleanup

	hash_table_delete(fundecls);
	hash_table_delete(string_store);
	
      }
      return error_count > 0;
    } else {
      printf("parse failed!\n");
      return 1;
    }    
  }

  if (!strcmp(argv[1], "-dot")) {
    if((yyin = fopen(argv[2], "r")) == NULL) {
      perror("an issue with reading models");
      exit(1);
    }
    if(!yyparse()) {
      //todo now
      if (program_root != NULL) {

	//first pass of typechecking: resolve names and collect string literals
	printf("Name resolution...\n");
	make_scope();
	scope_enter();
	decl_resolve(program_root);
	scope_exit();
	mark_program_symbols_with_rbppos(program_root);
	//mark_program_symbols_with_numbers(program_root);
	printf("Found %d error(s) in name resolution.\n", error_count);

	//second pass of typechecking: assign types
	int preverrs = error_count;
	printf("Typechecking...\n");
	decl_typecheck(program_root);
	printf("Found %d error(s) while typechecking.\n", error_count - preverrs);

	//print dot
	print_dot(program_root);
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
