CC		= gcc
CCOPTS  	= -g -Wall 

all:	parser.c scanner.c main.c
	$(CC) $(CCOPTS) parser.c scanner.c main.c -o bminor

parser.c parser.h: bminor.y 
	bison --defines=parser.h --output=parser.c -v bminor.y

scanner.c: bminor.l parser.h
	flex --outfile=scanner.c bminor.l

test:
	. tests/scanner/run_all_tests.sh

clean:
	rm -rf *~ *.yy.c *.o ltsNet *.tab.* *.dot \#* parser.h parser.c

