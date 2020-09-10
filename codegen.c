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
  int rbpoffset = 0;  
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

    switch (s->which) { //first six args go to registers
      //(I might have broken X86_64 stack param. convention, but I'm consistent elsewhere.)
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
    default:
      //then the next go to rbp+8*k
      //(byte vals too)
      rbpoffset = (s->which - 6) * 8 + 16;
      strcpy(nlab + offset, "rbp+");
      sprintf(nlab + offset + 4, "%d", rbpoffset);
      break;
    }
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

void expr_codegen(expr* e) { //todo

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
	printf("lea %s, %s\n", scratch_name(e->reg), symbol_codegen(e->symbol, 1));	
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
	printf("mov [%s], %s\n", scratch_name(e->left->reg), scratch_name(e->right->reg));
      }
    }
    //e->left->reg contains the address of array element/variable

    e->reg = e->left->reg;
    scratch_free(e->left->reg);
    scratch_free(e->right->reg);      
    break;
  }
      
  case EXPR_FUN_CALL: {

    //I have broken x86_64 stack param placement convention. I like it this way, so it is
    //staying broken, at least for now.

    expr* arg = e->right;
    int i = 0;

    while (arg != NULL) {
      expr_codegen(arg->left);
      if (i < 6) printf("mov %s, %s\n", argreg_name(i), scratch_name(arg->left->reg));
      else printf("push %s\n", scratch_name(arg->left->reg));
      scratch_free(arg->left->reg);
      arg = arg->right;
      ++i;
    }

    printf("call %s\n", e->left->name);
    if (i - 6 > 0) printf("add rsp, %d\n", (i-6)*8); //clear stack post-call

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
    printf("jmp .%s_epilogue\n", curr_function_name);
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

    expr_codegen(s->init_expr); 
    scratch_free(s->init_expr->reg);

    printf("%s:\n", label_name(for_begin_label, 'L'));
    expr_codegen(s->expr);
    printf("cmp %s, 0\n", scratch_name(s->expr->reg));
    scratch_free(s->expr->reg);
    printf("je %s\n", label_name(for_done_label, 'L'));

    stmt_codegen(s->body);
    expr_codegen(s->next_expr);
    scratch_free(s->next_expr->reg);

    printf("jmp %s\n", label_name(for_begin_label, 'L'));
    printf("%s:\n", label_name(for_done_label, 'L'));

    break;
  }

  
  //todo more cases

  }

  stmt_codegen(s->next);    

}

void decl_codegen(decl* d) {

  //TODO

  //todo - ustal curr_function_name w momencie deklaracji funkcji
  //etykiety do wyemitowania: .curr_function_name_{prologue,epilogue}

}
