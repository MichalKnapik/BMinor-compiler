#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "codegen.h"
#include "smalltools.h"
#include "codegen_tools.h"

const int SCRATCH_S = 7;
const char* name[] = {"rbx", "r10", "r11", "r12", "r13", "r14", "r15"};
reg_use inuse[] = {FREE, FREE, FREE, FREE, FREE, FREE, FREE};

const int ARG_REGS_S = 6;
const char* argregnames[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

int label_counter = 0;

extern struct hash_table* string_store;

int regname_to_number(const char* rname) {

  for (int i = 0; i < SCRATCH_S; ++i) {
    if (!strcmp(rname, argregnames[i])) return i;
  }

  assert(0);
  return -1;
}

const char* argreg_name(int r) {

  if (r >= 0 && r < ARG_REGS_S) return argregnames[r];

  int rbppos = (r - 6)*8 + 16;
  get_number_of_positions(r);
  char* nlab = calloc((get_number_of_positions(r) + 5), sizeof(char));    
  strcpy(nlab, "rbp+");
  sprintf(nlab + 4, "%d", rbppos);

  return nlab;
}

int scratch_alloc() {

  for (int i = 0; i < SCRATCH_S; ++i) {
    if (inuse[i] == FREE) {
      inuse[i]= INUSE;
      return i;
    }
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

void codegen_array_element_reference(expr* e) {

  //load the address of the beginning of the array
  expr_codegen(e->left);

  //the right node is the offset
  expr_codegen(e->right);

  //compute offset
  type* arrtype = e->left->symbol->type->subtype;
  int offset = type_size(arrtype);
  int tempreg = scratch_alloc();
  printf("imul %s, %s, %d\n", scratch_name(tempreg), scratch_name(e->right->reg), offset);
  printf("add %s, %s\n", scratch_name(e->left->reg), scratch_name(tempreg));
  e->reg = e->left->reg;
  scratch_free(tempreg);
  scratch_free(e->right->reg);

  //at the exit, e->reg contains the address of array element
}

void codegen_variable_reference(expr* e) {

  e->reg = scratch_alloc();

  switch (e->symbol->kind) {

  case SYMBOL_LOCAL: 
    printf("lea %s, %s\n", scratch_name(e->reg), symbol_codegen(e->symbol, 1));	
    break;

  case SYMBOL_GLOBAL:
    printf("mov %s, %s\n", scratch_name(e->reg), symbol_codegen(e->symbol, 0));
    break;

  case SYMBOL_PARAM:
    if (is_param_on_stack(e->symbol)) {
      printf("lea %s, %s\n", scratch_name(e->reg), symbol_codegen(e->symbol, 1));
    } else {
      e->reg = regname_to_number(symbol_codegen(e->symbol, 0));
    }
    break;

  }

  //at the exit, e->reg contains the address of the name (var)
}

int count_args(expr* e) {

  expr* arg = e->right;
  int i = 0;

  while (arg != NULL) {
    arg = arg->right;
    ++i;
  }
  
  return i;
}
