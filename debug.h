#ifndef DEBUG_H
#define DEBUG_H

#include "decl.h"
#include "stmt.h"
#include "param_list.h"

void print_mem_pos_decl(decl* d);
void print_mem_pos_params(param_list* p);
void print_mem_pos_stmt(stmt* s);

#endif
