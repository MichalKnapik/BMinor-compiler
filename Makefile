CC		= gcc
CCOPTS  	= -g -Wall 
BISOPTS	        = --report=all

all:	scanner_and_parser semantic_tools
	$(CC) $(CCOPTS) parser_and_scanner.o semantic_tools.o main.c -o bminor

scanner_and_parser: parser.c scanner.c
	$(CC) $(CCOPTS) parser.c scanner.c -c
	$(CC) $(CCOPTS) decl.c stmt.c expr.c type.c param_list.c smalltools.c -c
	ld -r parser.o scanner.o decl.o stmt.o expr.o type.o param_list.o smalltools.o -o parser_and_scanner.o

semantic_tools: 
	$(CC) $(CCOPTS) hash_table.c scope.c symbol.c -c
	ld -r scope.o symbol.o hash_table.o -o semantic_tools.o

parser.c parser.h: bminor.y 
	bison $(BISOPTS) --defines=parser.h --output=parser.c -v bminor.y

scanner.c: bminor.l parser.h
	flex --outfile=scanner.c bminor.l

test:
	. tests/scanner/run_all_tests.sh
	. tests/parser/run_all_tests.sh

clean:
	rm -rf *~ *.yy.c *.o ltsNet *.tab.* *.dot \#* parser.h parser.c

count:
	cat scope.* symbol.* decl.* stmt.* expr.* type.* param_list.* smalltools.* parser.* scanner.* bminor.l bminor.y main.c | wc -l
