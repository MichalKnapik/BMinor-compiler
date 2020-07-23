CC		= gcc
CCOPTS  	= -g -Wall 
BISOPTS	        = --report=all

all:	parser.c scanner.c 
	$(CC) $(CCOPTS) parser.c scanner.c decl.c stmt.c expr.c type.c param_list.c smalltools.c main.c -o bminor

parser.c parser.h: bminor.y 
	bison $(BISOPTS) --defines=parser.h --output=parser.c -v bminor.y

scanner.c: bminor.l parser.h
	flex --outfile=scanner.c bminor.l

test:
	. tests/scanner/run_all_tests.sh
	. tests/parser/run_all_tests.sh

clean:
	rm -rf *~ *.yy.c *.o ltsNet *.tab.* *.dot \#* parser.h parser.c

