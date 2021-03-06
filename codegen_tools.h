#ifndef CODEGEN_TOOLS_H
#define CODEGEN_TOOLS_H

#include "hash_table.h"
#include "decl.h"
#include "expr.h"

typedef enum {INUSE, FREE} reg_use;

int regname_to_number(const char* rname);
const char* argreg_name(int r);
int scratch_alloc();
void scratch_free(int r);
const char* scratch_name(int r);
const char* scratch_name_low8(int r);
int label_create();
const char* label_name(int label, char fletter);
int count_args(expr* e);
void print_global_array_elts(decl* d);
const char* rbp_offset(int offset);
void codegen_array_element_reference(expr* e);
void codegen_variable_reference(expr* e);
void codegen_program_preamble();
void codegen_program_epilogue();  
void codegen_program_externs();
void rax_alloc(expr* e);

#endif
