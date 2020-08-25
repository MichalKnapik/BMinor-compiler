#ifndef CODEGEN_H
#define CODEGEN_H

#include "symbol.h"

int scratch_alloc();
void scratch_free(int r);
const char* scratch_name(int r);
int label_create();
const char* label_name(int label);
const char* symbol_codegen(symbol* s);

#endif
