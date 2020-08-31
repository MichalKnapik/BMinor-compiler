#include "codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "smalltools.h"
#include "hash_table.h"
#include "assert.h"

typedef enum {INUSE, FREE} reg_use;

const int SCRATCH_S = 7;
const char* name[] = {"rbx", "r10", "r11", "r12", "r13", "r14", "r15"};
reg_use inuse[] = {FREE, FREE, FREE, FREE, FREE, FREE, FREE};
int label_counter = 0;

extern struct hash_table* string_store;

int scratch_alloc() {

  for (int i = 0; i < SCRATCH_S; ++i) {
    if (inuse[i] == FREE) return i;
  }

  printf("Error (for now): out of scratch registers.\n");
  exit(1);

  return -1;
}

void scratch_free(int r) {

  if (r < 0 || r >= SCRATCH_S) {
    printf("Error: no register numbered %d.\n", r);
    exit(1);
  }
  
  inuse[r] = FREE;
}

const char* scratch_name(int r) {

  if (r < 0 || r >= SCRATCH_S) {
    printf("Error: no register numbered %d.\n", r);
    exit(1);
  }

  return name[r];
}

int label_create() {
  return label_counter++;
}

const char* label_name(int label, char fletter) {

  char* nlab = calloc((get_number_of_positions(label)+2), sizeof(char));
  nlab[0] = fletter;
  sprintf(nlab+1, "%d", label);

  return nlab;
}

const char* symbol_codegen(symbol* s) {
  
  if (s->kind == SYMBOL_GLOBAL) return s->name;
  
  if (s->kind == SYMBOL_PARAM) { //x86_64 calling convention

    switch (s->which) { //first six args go to registers
    case 0:
      return "rdi";
      break;
    case 1:
      return "rsi";
      break;
    case 2:
      return "rdx";
      break;
    case 3:
      return "rcx";
      break;
    case 4:
      return "r8";
      break;
    case 5:
      return "r9";
      break;
    }
    
    //further args go to stacks 
    int rbpoffset = (s->which - 6) * 8 + 16;
    char* nlab = calloc((get_number_of_positions(rbpoffset)+5), sizeof(char));
    strcpy(nlab, "rbp+");
    sprintf(nlab+4, "%d", rbpoffset);
    return nlab;
  }

  //s->kind == SYMBOL_LOCAL
  char* nlab = calloc((get_number_of_positions(s->which)+5), sizeof(char));
  strcpy(nlab, "rbp-");
  sprintf(nlab+4, "%d", s->which);
  return nlab;  

}

const char* string_literal_codegen(const char* str) {

  int* strpos = hash_table_lookup(string_store, str);
  assert(strpos != NULL);

  return label_name(*strpos, 'S');
}

void string_store_codegen() {

	char* key = NULL;
	int* value = NULL;
	hash_table_firstkey(string_store);

	while (hash_table_nextkey(string_store, &key, (void**) &value))
	  printf("\t%s db \"%s\", 0\n", string_literal_codegen(key), key);
}

void function_prologue_codegen(decl* d) {

  printf("\tpush rbp\n");
  printf("\tmov rbp, rsp\n");  

  //alloc stack space
  printf("\tsub rsp, %d\n", d->rbpoffset);

  //non-optimised, saves all callee-saved registers
  printf("\tpush rbx\n");
  printf("\tpush r12\n");
  printf("\tpush r13\n");
  printf("\tpush r14\n");     
  printf("\tpush r15\n");     		       

}

void function_epilogue_codegen(decl* d) {

  //non-optimised, restores all callee-saved registers
  printf("\tpop r15\n");     		       
  printf("\tpop r14\n");     
  printf("\tpop r13\n");
  printf("\tpop r12\n");
  printf("\tpop rbx\n");

  //de-alloc stack space
  printf("\tmov rsp, rbp\n");

  printf("\tpop rbp\n");
  printf("\tret\n");  
  
}
