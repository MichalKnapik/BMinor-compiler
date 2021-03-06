#ifndef CODEGEN_H
#define CODEGEN_H

#include "symbol.h"
#include "decl.h"
#define YASM

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
void caller_save_registers();
void caller_restore_registers();

void expr_codegen(expr* e);
void stmt_codegen(stmt* s);
void decl_codegen(decl* d);

void codegen_stack_fun_call_args(expr* arg);

#endif
