#include "codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "smalltools.h"

typedef enum {INUSE, FREE} reg_use;

const int SCRATCH_S = 7;
const char* name[] = {"rbx", "r10", "r11", "r12", "r13", "r14", "r15"};
reg_use inuse[] = {FREE, FREE, FREE, FREE, FREE, FREE, FREE};
int label_counter = 0;

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

const char* label_name(int label) {

  char* nlab = calloc((get_number_of_positions(label)+2), sizeof(char));
  nlab[0] = 'L';
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

    int rbpoffset = (s->which - 6) * 8 + 16;
    char* nlab = calloc((get_number_of_positions(rbpoffset)+5), sizeof(char));
    strcpy(nlab, "rbp+");
    sprintf(nlab+4, "%d", rbpoffset);
    return nlab;

  }

  //s->kind == SYMBOL_LOCAL
  //local fun definitions are not allowed, so this must be a local var decl/use

  //plain var

  //array - first compute its size
  
  //todo
  //note 1 - I think that array size should be preserved!
  //note 2 - not neccesarily, because non-inited arrays don't exist
  //note 3 - maybe still
}
