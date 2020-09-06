#ifndef CODEGEN_H
#define CODEGEN_H

#include "symbol.h"
#include "decl.h"

int scratch_alloc();
void scratch_free(int r);
const char* scratch_name(int r);
int label_create();
const char* label_name(int label, char fletter);

int regname_to_number(const char* rname);

const char* symbol_codegen(symbol* s, int deref);
const char* string_literal_codegen(const char* str);
void string_store_codegen();

void function_prologue_codegen(decl* d);
void function_epilogue_codegen(decl* d);

void expr_codegen(expr* e);

void codegen_array_element_reference(expr* e);
void codegen_variable_reference(expr* e);

#endif
