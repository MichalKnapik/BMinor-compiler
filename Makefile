CC		= gcc
CCOPTS  	= -g -Wall 
BISOPTS	        = --report=all
ASM             = nasm

all:	scanner_and_parser semantic_tools codegen
	$(CC) $(CCOPTS) parser_and_scanner.o semantic_tools.o pass_and_codegen.o main.c -o bminor

codegen:
	$(CC) $(CCOPTS) stack_rbp_pass.c codegen.c debug.c codegen_tools.c -c
	ld -r stack_rbp_pass.o codegen.o codegen_tools.o debug.o -o pass_and_codegen.o

scanner_and_parser: parser.c scanner.c
	$(CC) $(CCOPTS) parser.c scanner.c -c
	$(CC) $(CCOPTS) decl.c stmt.c expr.c type.c param_list.c smalltools.c -c
	ld -r parser.o scanner.o decl.o stmt.o expr.o type.o param_list.o smalltools.o -o parser_and_scanner.o

semantic_tools: 
	$(CC) $(CCOPTS) hash_table.c scope.c symbol.c name_resolution.c type_check.c -c
	ld -r scope.o symbol.o hash_table.o name_resolution.o type_check.o -o semantic_tools.o 

parser.c parser.h: bminor.y 
	bison $(BISOPTS) --defines=parser.h --output=parser.c -v bminor.y

scanner.c: bminor.l parser.h
	flex --outfile=scanner.c bminor.l

compile:
	gcc -no-pie -Wall -g -c library.c 
	$(ASM) -f elf64 $(src).asm
	gcc -no-pie -g -m64 -lc library.o $(src).o -o $(src).bin

clean:
	rm -rf *~ *.yy.c *.o ltsNet *.tab.* *.dot \#* parser.h parser.c \#* *.lst
