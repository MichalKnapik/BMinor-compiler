#include <string.h>
#include <stdio.h>
#include "stack_rbp_pass.h"
#include "name_resolution.h"

void fix_string_sizes_decl(decl* d) {

  if (d == NULL) return;

  if (d->type->kind == TYPE_STRING && d->value != NULL)
    d->type->arrsize = strlen(d->value->string_literal) + 1; //+1 for null symbol

  if (d->code != NULL) fix_string_sizes_stmt(d->code);

  fix_string_sizes_decl(d->next);

}

void fix_string_sizes_stmt(stmt* s) {

  if (s == NULL) return;

  fix_string_sizes_decl(s->decl);
  fix_string_sizes_stmt(s->body);  
  fix_string_sizes_stmt(s->else_body);
  fix_string_sizes_stmt(s->next);

}

void mark_program_symbols_with_rbppos(decl* d) {

  if (d == NULL) return;
  fix_string_sizes_decl(d);
  int current_rbp_offset = 0;
  mark_symbols_with_rbppos_decl(d, &current_rbp_offset);

}

void mark_symbols_with_rbppos_decl(decl* d, int* current_rbp_offset) {

  /* Stack alignment not implemented. */

  if (d == NULL) return;

  if (d->code != NULL) { //diving into a function
    int parscount = 0;
    //parameters are either quads or refs (quads too), so need only numbers
    mark_symbols_with_numbers_type(d->type, &parscount);
    *current_rbp_offset = 0;    
    mark_symbols_with_rbppos_stmt(d->code, current_rbp_offset);
  }

  //local var decl
  else if (d->symbol->kind == SYMBOL_LOCAL) {
    
    if (d->symbol->which < 0) { //marking new symbol

      if (d->symbol->type->kind == TYPE_ARRAY) {
	switch (d->symbol->type->subtype->kind) {
	case TYPE_CHARACTER:
	case TYPE_BOOLEAN: //bools are bytes
	  d->symbol->which = (*current_rbp_offset) + d->symbol->type->arrsize;
	  break;
	  //other cases are quads as, like in C, array of strings is an array of refs, etc
	default:
	  d->symbol->which = (*current_rbp_offset) + d->symbol->type->arrsize*8;
	}
      }
      
      else if (d->symbol->type->kind == TYPE_BOOLEAN || d->symbol->type->kind == TYPE_CHARACTER)
	d->symbol->which = (*current_rbp_offset) + 1;

      else if (d->symbol->type->kind == TYPE_INTEGER || d->symbol->type->kind == TYPE_STRING) 
	d->symbol->which = (*current_rbp_offset) + 8; //strings are refs in data section

      (*current_rbp_offset) = d->symbol->which;
    }      
  }
  d->rbpoffset = (*current_rbp_offset);

  mark_symbols_with_rbppos_decl(d->next, current_rbp_offset);
  
}

void mark_symbols_with_rbppos_stmt(stmt* s, int* current_rbp_offset) {

  if (s == NULL) return;

  mark_symbols_with_rbppos_decl(s->decl, current_rbp_offset);
  mark_symbols_with_rbppos_stmt(s->body, current_rbp_offset);  
  mark_symbols_with_rbppos_stmt(s->else_body, current_rbp_offset);

  mark_symbols_with_rbppos_stmt(s->next, current_rbp_offset);
  
}
