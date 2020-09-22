#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "smalltools.h"
#include "hash_table.h"
#include "codegen_tools.h"
#include "codegen.h"

extern struct hash_table* string_store;
static const char* curr_function_name = NULL;

const char* symbol_codegen(symbol* s, int deref) {
  
  char* nlab = calloc((strlen(s->name) + 10), sizeof(char));
  int offset = 0;
  if (deref != 0) {
    nlab[0] = '[';
    offset = 1;
  }

  if (s->kind == SYMBOL_GLOBAL) {
    strcpy(nlab + offset, s->name);
    if (deref != 0) nlab[strlen(nlab)] = ']';
    return nlab;
  }
  
  if (s->kind == SYMBOL_PARAM) { //x86_64 calling convention
    if (s->which < 6) return argreg_name(s->which); //arg regs are not deref'd
    strcpy(nlab + offset, argreg_name(s->which));    
    if (deref != 0) nlab[strlen(nlab)] = ']';
    return nlab;
  }

  //s->kind == SYMBOL_LOCAL
  strcpy(nlab + offset, "rbp-");
  sprintf(nlab + offset + 4, "%d", s->which);
  if (deref != 0) nlab[strlen(nlab)] = ']';  

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
    printf("%s db `%s`, 0\n", string_literal_codegen(key), key);  
}

void function_prologue_codegen(decl* d) {

  printf("push rbp\n");
  printf("mov rbp, rsp\n");  

  //alloc stack space
  printf("sub rsp, %d\n", d->rbpoffset);

  //non-optimised, saves all callee-saved registers
  printf("push rbx\n");
  printf("push r12\n");
  printf("push r13\n");
  printf("push r14\n");     
  printf("push r15\n");     		       

}

void function_epilogue_codegen(decl* d) {

  //non-optimised, restores all callee-saved registers
  printf("pop r15\n");     		       
  printf("pop r14\n");     
  printf("pop r13\n");
  printf("pop r12\n");
  printf("pop rbx\n");

  //de-alloc stack space
  printf("mov rsp, rbp\n");

  printf("pop rbp\n");
  printf("ret\n");  
  
}

void caller_save_registers() {

  //non-optimised, saves all caller-saved registers
  //push the arguments
  printf("push rdi\n");
  printf("push rsi\n");
  printf("push rdx\n");
  printf("push rcx\n");
  printf("push r8\n");
  printf("push r9\n");

  //push temps
  printf("push r10\n");
  printf("push r11\n");

}

void caller_restore_registers() {

  //non-optimised, restores all caller-saved registers
  //pop temps
  printf("pop r11\n");
  printf("pop r10\n");

  //pop the arguments
  printf("pop r9\n");
  printf("pop r8\n");
  printf("pop rcx\n");
  printf("pop rdx\n");
  printf("pop rsi\n");
  printf("pop rdi\n");

}


void expr_codegen(expr* e) {

  //This is a non-optimised, somewhat RISC-y expression codegen routine for X86_64.

  if (e == NULL) return;

  switch (e->kind) {

    //**leaf nodes**
  case EXPR_STR:
    //load reference to a string literal from the string store
    e->reg = scratch_alloc();
    printf("mov %s, %s\n", scratch_name(e->reg), string_literal_codegen(e->string_literal));
    break;

  case EXPR_INT:
  case EXPR_BOOL:
  case EXPR_CHAR:
    //load immediates - non-optimised, all goes to quad-word registers
    e->reg = scratch_alloc();
    printf("mov %s, %d\n", scratch_name(e->reg), e->literal_value);
    break;

  case EXPR_NAME: {
    e->reg = scratch_alloc();

    //for strings (there are no dynamic strings), only address is held at the stack and strings sit in storage
    //this also works for arrays, where it loads the beginning address
    if (e->symbol->type->kind == TYPE_STRING || e->symbol->type->kind == TYPE_ARRAY) {

      switch (e->symbol->kind) {

      case SYMBOL_LOCAL: //stack storage
	//probably paying for bad design here
	if (e->symbol->type->kind == TYPE_STRING)	
	printf("mov %s, %s\n", scratch_name(e->reg), symbol_codegen(e->symbol, 1));
	else printf("lea %s, %s\n", scratch_name(e->reg), symbol_codegen(e->symbol, 1));	
	break;

      case SYMBOL_GLOBAL: //copying label's address
	printf("mov %s, %s\n", scratch_name(e->reg), symbol_codegen(e->symbol, 0));
	break;

      case SYMBOL_PARAM: //string/array's address is either in a reg or [rbp+xxx]
	printf("mov %s, %s\n", scratch_name(e->reg), symbol_codegen(e->symbol, 1));
	break;
      }

    }

    else { //the name is a non-string/array, so move the literal value to scratch reg
      if ((e->symbol->type->kind == TYPE_BOOLEAN || e->symbol->type->kind == TYPE_CHARACTER)
	  && e->symbol->kind != SYMBOL_PARAM) { //byte size
	printf("movzx %s, byte %s\n", scratch_name(e->reg), symbol_codegen(e->symbol, 1));
      }
      else { //quad size
	printf("mov %s, %s\n", scratch_name(e->reg), symbol_codegen(e->symbol, 1));
      }
    }

    break;
  }
    //**inner nodes**
    //a convention for nasm (different than the book's gas conv.) - re-use left register

    //arithmetics and logic instructions
  case EXPR_ADD:
    expr_codegen(e->left);
    expr_codegen(e->right);
    printf("add %s, %s\n", scratch_name(e->left->reg), scratch_name(e->right->reg));
    e->reg = e->left->reg;
    scratch_free(e->right->reg);
    break;

  case EXPR_SUB:
    expr_codegen(e->left);
    expr_codegen(e->right);
    printf("sub %s, %s\n", scratch_name(e->left->reg), scratch_name(e->right->reg));
    e->reg = e->left->reg;
    scratch_free(e->right->reg);
    break;

  case EXPR_MUL:
    expr_codegen(e->left);
    expr_codegen(e->right);
    printf("imul %s, %s\n", scratch_name(e->left->reg), scratch_name(e->right->reg));
    e->reg = e->left->reg;
    scratch_free(e->right->reg);
    break;

  case EXPR_DIV:
    //need to save rdx and rax
    expr_codegen(e->left);
    expr_codegen(e->right);
    printf("push rdx\n");
    printf("push rax\n");
    printf("mov rax, %s\n", scratch_name(e->left->reg));
    printf("cqo\n");
    printf("idiv %s\n", scratch_name(e->right->reg));
    printf("mov %s, rax\n", scratch_name(e->left->reg));
    e->reg = e->left->reg;
    scratch_free(e->right->reg);
    printf("pop rax\n");
    printf("pop rdx\n");
    break;

  case EXPR_MOD:
    //need to save rdx and rax
    expr_codegen(e->left);
    expr_codegen(e->right);
    printf("push rdx\n");
    printf("push rax\n");
    printf("mov rax, %s\n", scratch_name(e->left->reg));
    printf("cqo\n");
    printf("idiv %s\n", scratch_name(e->right->reg));
    printf("mov %s, rdx\n", scratch_name(e->left->reg));
    e->reg = e->left->reg;
    scratch_free(e->right->reg);
    printf("pop rax\n");
    printf("pop rdx\n");
    break;

  case EXPR_EXP:
    //emit extern integer_power in src .text
    expr_codegen(e->left);
    expr_codegen(e->right);
    printf("mov rdi, %s\n", scratch_name(e->left->reg));
    printf("mov rsi, %s\n", scratch_name(e->right->reg));
    printf("call integer_power\n");
    printf("mov %s, rax\n", scratch_name(e->left->reg));
    e->reg = e->left->reg;
    scratch_free(e->right->reg);
    break;

  case EXPR_UN_MIN:
    //there must be a better way...
    printf("push rdx\n");
    printf("push rax\n");
    expr_codegen(e->left);
    printf("imul %s, -1\n", scratch_name(e->left->reg));
    e->reg = e->left->reg;
    printf("pop rax\n");
    printf("pop rdx\n");
    break;

  case EXPR_INC:
    expr_codegen(e->left);
    //the expression's result
    e->reg = e->left->reg;
    //the side effect
    printf("inc qword %s\n", symbol_codegen(e->left->symbol, 1));
    break;

  case EXPR_DEC:
    expr_codegen(e->left);
    //the expression's result
    e->reg = e->left->reg;
    //the side effect
    printf("dec qword %s\n", symbol_codegen(e->left->symbol, 1));
    break;

  case EXPR_LE:
    expr_codegen(e->left);
    expr_codegen(e->right);
    printf("cmp %s, %s\n", scratch_name(e->left->reg), scratch_name(e->right->reg));
    const char* jlabel_le = label_name(label_create(), 'L');
    const char* jlabel_end_le = label_name(label_create(), 'L');
    printf("jle %s\n", jlabel_le);
    printf("mov %s, 0\n", scratch_name(e->left->reg));
    printf("jmp %s\n", jlabel_end_le);
    printf("%s:\n", jlabel_le);
    printf("mov %s, 1\n", scratch_name(e->left->reg));
    printf("%s:\n", jlabel_end_le);
    e->reg = e->left->reg;
    scratch_free(e->right->reg);
    break;

  case EXPR_LT:
    expr_codegen(e->left);
    expr_codegen(e->right);
    printf("cmp %s, %s\n", scratch_name(e->left->reg), scratch_name(e->right->reg));
    const char* jlabel_lt = label_name(label_create(), 'L');
    const char* jlabel_end_lt = label_name(label_create(), 'L');
    printf("jl %s\n", jlabel_lt);
    printf("mov %s, 0\n", scratch_name(e->left->reg));
    printf("jmp %s\n", jlabel_end_lt);
    printf("%s:\n", jlabel_lt);
    printf("mov %s, 1\n", scratch_name(e->left->reg));
    printf("%s:\n", jlabel_end_lt);
    e->reg = e->left->reg;
    scratch_free(e->right->reg);
    break;

  case EXPR_GE:
    expr_codegen(e->left);
    expr_codegen(e->right);
    printf("cmp %s, %s\n", scratch_name(e->left->reg), scratch_name(e->right->reg));
    const char* jlabel_ge = label_name(label_create(), 'L');
    const char* jlabel_end_ge = label_name(label_create(), 'L');
    printf("jge %s\n", jlabel_ge);
    printf("mov %s, 0\n", scratch_name(e->left->reg));
    printf("jmp %s\n", jlabel_end_ge);
    printf("%s:\n", jlabel_ge);
    printf("mov %s, 1\n", scratch_name(e->left->reg));
    printf("%s:\n", jlabel_end_ge);
    e->reg = e->left->reg;
    scratch_free(e->right->reg);
    break;

  case EXPR_GT:
    expr_codegen(e->left);
    expr_codegen(e->right);
    printf("cmp %s, %s\n", scratch_name(e->left->reg), scratch_name(e->right->reg));
    const char* jlabel_gt = label_name(label_create(), 'L');
    const char* jlabel_end_gt = label_name(label_create(), 'L');
    printf("jg %s\n", jlabel_gt);
    printf("mov %s, 0\n", scratch_name(e->left->reg));
    printf("jmp %s\n", jlabel_end_gt);
    printf("%s:\n", jlabel_gt);
    printf("mov %s, 1\n", scratch_name(e->left->reg));
    printf("%s:\n", jlabel_end_gt);
    e->reg = e->left->reg;
    scratch_free(e->right->reg);
    break;

  case EXPR_EQ:
    expr_codegen(e->left);
    expr_codegen(e->right);
    printf("cmp %s, %s\n", scratch_name(e->left->reg), scratch_name(e->right->reg));
    const char* jlabel_eq = label_name(label_create(), 'L');
    const char* jlabel_end_eq = label_name(label_create(), 'L');
    printf("je %s\n", jlabel_eq);
    printf("mov %s, 0\n", scratch_name(e->left->reg));
    printf("jmp %s\n", jlabel_end_eq);
    printf("%s:\n", jlabel_eq);
    printf("mov %s, 1\n", scratch_name(e->left->reg));
    printf("%s:\n", jlabel_end_eq);
    e->reg = e->left->reg;
    scratch_free(e->right->reg);
    break;

  case EXPR_NEQ:
    expr_codegen(e->left);
    expr_codegen(e->right);
    printf("cmp %s, %s\n", scratch_name(e->left->reg), scratch_name(e->right->reg));
    const char* jlabel_neq = label_name(label_create(), 'L');
    const char* jlabel_end_neq = label_name(label_create(), 'L');
    printf("jne %s\n", jlabel_neq);
    printf("mov %s, 0\n", scratch_name(e->left->reg));
    printf("jmp %s\n", jlabel_end_neq);
    printf("%s:\n", jlabel_neq);
    printf("mov %s, 1\n", scratch_name(e->left->reg));
    printf("%s:\n", jlabel_end_neq);
    e->reg = e->left->reg;
    scratch_free(e->right->reg);
    break;

  case EXPR_AND:
    expr_codegen(e->left);
    expr_codegen(e->right);
    printf("and %s, %s\n", scratch_name(e->left->reg), scratch_name(e->right->reg));
    e->reg = e->left->reg;
    scratch_free(e->right->reg);
    break;

  case EXPR_OR:
    expr_codegen(e->left);
    expr_codegen(e->right);
    printf("or %s, %s\n", scratch_name(e->left->reg), scratch_name(e->right->reg));
    e->reg = e->left->reg;
    scratch_free(e->right->reg);
    break;

  case EXPR_NEG:
    expr_codegen(e->left);
    printf("not %s\n", scratch_name(e->left->reg));
    printf("and %s, 0x1\n", scratch_name(e->left->reg));
    e->reg = e->left->reg;
    break;

  case EXPR_ARG:
    expr_codegen(e->left);
    e->reg = e->left->reg;    
    break;

  case EXPR_ARR_SUBS: {

    codegen_array_element_reference(e);
    //e->left->reg contains the address of array element

    type_t arrkind = e->left->symbol->type->subtype->kind;

    //dereference the element
    if (arrkind == TYPE_BOOLEAN || arrkind == TYPE_CHARACTER) { //byte-size element
      printf("movzx %s, byte [%s]\n", scratch_name(e->left->reg), scratch_name(e->left->reg));
    } else { //quad-size element
      printf("mov %s, [%s]\n", scratch_name(e->left->reg), scratch_name(e->left->reg));    
    }

    break;
  }

  case EXPR_ASSGN: {

    //this case is too messy

    if (e->left->kind == EXPR_ARR_SUBS) { //assignment to array's cell
      codegen_array_element_reference(e->left);
      expr_codegen(e->right);
      printf("mov [%s], %s\n", scratch_name(e->left->reg), scratch_name(e->right->reg));
    }

    else { //assignment to variable

      if (e->left->symbol->kind == SYMBOL_PARAM) { //parameters need special handling
	codegen_variable_reference(e->left);
	expr_codegen(e->right);
	//e->right->reg contains the data (or ref to string, because refs to arrays are forbidden)
	if (is_param_on_stack(e->left->symbol))
	  printf("mov [%s], %s\n", scratch_name(e->left->reg), scratch_name(e->right->reg));
	else
	  printf("mov %s, %s\n", argreg_name(e->left->reg), scratch_name(e->right->reg));	    

      } else { //global or local vars
	codegen_variable_reference(e->left);
	expr_codegen(e->right);
	//e->right->reg contains the data (or ref to string, because refs to arrays are forbidden)

	if (e->left->symbol->type->kind == TYPE_BOOLEAN || e->left->symbol->type->kind == TYPE_CHARACTER) { //bytes
	  printf("mov [%s], %s\n", scratch_name(e->left->reg), scratch_name_low8(e->right->reg));
	}
	else { //quads
	  printf("mov [%s], %s\n", scratch_name(e->left->reg), scratch_name(e->right->reg));
	}

      }
    }
    //e->left->reg contains the address of array element/variable

    e->reg = e->left->reg;
    scratch_free(e->left->reg);
    scratch_free(e->right->reg);      
    break;
  }
      
  case EXPR_FUN_CALL: {

    expr* arg = e->right;
    int i = 0;

    caller_save_registers();

    while (arg != NULL && i < 6) {
      expr_codegen(arg->left);
      printf("mov %s, %s\n", argreg_name(i), scratch_name(arg->left->reg));
      scratch_free(arg->left->reg);
      arg = arg->right;
      ++i;
    }

    codegen_stack_fun_call_args(arg);

    printf("call %s\n", e->left->name);
    if (i - 6 > 0) printf("add rsp, %d\n", (i-6)*8); //clear stack post-call

    caller_restore_registers();

    break;
  }

  }
  
}

void stmt_codegen(stmt* s) {

  if (s == NULL) return;

  switch (s->kind) {

  case STMT_DECL:
    decl_codegen(s->decl);
    break;

  case STMT_EXPR:
    expr_codegen(s->expr);
    scratch_free(s->expr->reg);
    break;

  case STMT_BLOCK:
    stmt_codegen(s->body);
    break;

  case STMT_RETURN: 
    expr_codegen(s->expr);
    printf("mov rax, %s\n", scratch_name(s->expr->reg));
    printf("jmp %s_epilogue\n", curr_function_name);
    scratch_free(s->expr->reg);
    break;
  
  case STMT_IF_ELSE: {

    int else_label = label_create();
    int done_label = label_create();

    expr_codegen(s->expr);
    printf("cmp %s, 0\n", scratch_name(s->expr->reg));
    scratch_free(s->expr->reg);
    printf("je %s\n", label_name(else_label, 'L'));
    stmt_codegen(s->body);
    printf("jmp %s\n", label_name(done_label, 'L'));
    printf("%s:\n", label_name(else_label, 'L'));    
    stmt_codegen(s->else_body);
    printf("%s:\n", label_name(done_label, 'L'));
    break;
  }

  case STMT_FOR: {
    int for_begin_label = label_create(); 
    int for_done_label = label_create();

    if (s->init_expr != NULL) {
      expr_codegen(s->init_expr); 
      scratch_free(s->init_expr->reg);
    }

    printf("%s:\n", label_name(for_begin_label, 'L'));
    if (s->expr != NULL) {
      expr_codegen(s->expr);
      printf("cmp %s, 0\n", scratch_name(s->expr->reg));
      scratch_free(s->expr->reg);
      printf("je %s\n", label_name(for_done_label, 'L'));
    }

    stmt_codegen(s->body);

    if (s->next_expr != NULL) {
      expr_codegen(s->next_expr);
      scratch_free(s->next_expr->reg);
    }

    printf("jmp %s\n", label_name(for_begin_label, 'L'));
    printf("%s:\n", label_name(for_done_label, 'L'));

    break;
  }

  case STMT_PRINT: {

    expr* print_expr = s->expr;
    while (print_expr != NULL) {

      expr* val = print_expr->left;

      expr_codegen(val);
      caller_save_registers();
      printf("mov rdi, %s\n", scratch_name(s->expr->left->reg));


      if (is_expr_string(val) || (val->kind == EXPR_NAME && val->symbol->type->kind == TYPE_STRING)) 
	printf("call print_string\n");
      else
	if (is_expr_int(val)  || (val->kind == EXPR_NAME && val->symbol->type->kind == TYPE_INTEGER))
	  printf("call print_integer\n");
	else
	  if (is_expr_char(val) || (val->kind == EXPR_NAME && val->symbol->type->kind == TYPE_CHARACTER))
	    printf("call print_character\n");
	  else
	    if (is_expr_bool(val) || (val->kind == EXPR_NAME && val->symbol->type->kind == TYPE_BOOLEAN))
	      printf("call print_boolean\n");
	    else {
	      printf("Error: currently can print only strings, chars, bools, and ints. Cowardly exiting.\n");
	      exit(1);
	    }
      scratch_free(val->reg);
      caller_restore_registers();
      print_expr = print_expr->right;
    }
  }

  }

  stmt_codegen(s->next);    

}

void decl_codegen(decl* d) {

  if (d == NULL) return;

  switch (d->symbol->kind) {

  case SYMBOL_GLOBAL:

    //nasm/yasm can take fragmented
    //data sections; when rewriting consolidate this

    if (d->type->kind == TYPE_FUNCTION) { //fun decls

      printf("section .text\n");
      curr_function_name = d->name;
      printf("global %s\n", d->name);
      printf("%s:\n", d->name);      
      function_prologue_codegen(d);
      stmt_codegen(d->code);
      printf("%s_epilogue:\n", d->name);
      function_epilogue_codegen(d);      

    } else { //global variable decls

      printf("section .data\n");

      int is_error = 0;
      switch (d->type->kind) {

      case TYPE_INTEGER:
	if (d->value->kind != EXPR_INT) {is_error = 1; break;}
	printf("%s dq %d\n", d->name, d->value->literal_value);
	break;
      case TYPE_BOOLEAN:
	if (d->value->kind != EXPR_BOOL) {is_error = 1; break;}
	printf("%s db %d\n", d->name, d->value->literal_value);
	break;
      case TYPE_CHARACTER:
	if (d->value->kind != EXPR_CHAR) {is_error = 1; break;}
	printf("%s db %d\n", d->name, d->value->literal_value);
	break;
      case TYPE_STRING:
	if (d->value->kind != EXPR_STR) {is_error = 1; break;}
	//this is wasteful, because we already have string store
	//(but you could make it into a feature by saying that global
	//strings can be made mutable, etc.)
	printf("%s db \"%s\",0\n", d->name, d->value->string_literal);
	break;
      case TYPE_ARRAY: {
	//needs to be called after string_store_codegen
	print_global_array_elts(d);
	break;
      }
      default:
	printf("Error: disallowed declaration, exiting.\n");
	exit(1);
      }

      if (is_error) {
	printf("Error: currently can init globals only with static vals. Cowardly exiting.\n");
	exit(1);
      }
			     
    }
    break;

  case SYMBOL_LOCAL: {
    //todo now - init local vars
    //no local fun decls, only vars
    expr* initex = d->value;
    switch (d->type->kind) {

    case TYPE_STRING:
    case TYPE_INTEGER: {
      expr_codegen(initex);
      printf("mov %s, %s\n", symbol_codegen(d->symbol, 1), scratch_name(initex->reg));
      scratch_free(initex->reg);      
      break;
    }
    case TYPE_BOOLEAN:
    case TYPE_CHARACTER:
      expr_codegen(initex);
      printf("mov %s, %s\n", symbol_codegen(d->symbol, 1), scratch_name_low8(initex->reg));
      scratch_free(initex->reg);      
      break;
    case TYPE_ARRAY: {
      if (initex == NULL) {
	//if you want default inits, put it here
	break;
      }
      int stackoffset = -d->symbol->which;
      int tsize = type_size(d->type->subtype);
      while (initex != NULL) {
	expr_codegen(initex);
	if (tsize == 8) printf("mov [%s], %s\n", rbp_offset(stackoffset), scratch_name(initex->reg));
	else printf("mov [%s], %s\n", rbp_offset(stackoffset), scratch_name_low8(initex->reg));
	scratch_free(initex->reg);      
	stackoffset += tsize;
	initex = initex->right;
      }

      break;
    }
    default:
      printf("Error: disallowed local declaration, exiting.\n");
      exit(1);
    }
    break;
  
    default:
      printf("Error: disallowed declaration, exiting.\n");
      exit(1);
      break;
  }
  }

  decl_codegen(d->next);
}

void codegen_stack_fun_call_args(expr* arg) {

  //using compiler's stack instead of using own
  if (arg == NULL) return;
  codegen_stack_fun_call_args(arg->right);

  expr_codegen(arg->left);
  printf("push %s\n", scratch_name(arg->left->reg));
  scratch_free(arg->left->reg);

}

