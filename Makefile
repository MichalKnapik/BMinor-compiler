CC		= gcc
CCOPTS  	= -g -Wall 
BISOPTS	        = --report=all

all:	scanner_and_parser semantic_tools codegen
	$(CC) $(CCOPTS) parser_and_scanner.o semantic_tools.o codegen.o main.c -o bminor

codegen:
	$(CC) $(CCOPTS) codegen.c -c	

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

test:
	. tests/scanner/run_all_tests.sh
	. tests/parser/run_all_tests.sh
	. tests/typecheck/run_all_tests.sh

clean:
	rm -rf *~ *.yy.c *.o ltsNet *.tab.* *.dot \#* parser.h parser.c

count:
	cat scope.* symbol.* decl.* stmt.* expr.* type.* param_list.* smalltools.* parser.* scanner.* bminor.l bminor.y main.c name_resolution.* type_check.* | wc -l

check:
	./bminor -typecheck good.bminor > d.dot
	sed -i -n '5,10000p' d.dot
	dot -Tpdf d.dot -o d.pdf
	evince d.pdf &

