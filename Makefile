
objects = test_suite.o \
	linked_lists.o \
	hash_tables.o \
	intmdt_code_gen.o
name = linked_lists

CFLAGS = -g -Wall -Wextra -Werror -pedantic -O -g -std=c99

#default : $(objects)
#	gcc -o $(name) $(objects)

test : $(objects)
	gcc -o test_suite $(objects)

test_suite.o : linked_lists.h hash_tables.h intmdt_code_gen.h

intmdt_code_gen.o : intmdt_code_gen.h

linked_lists.o : linked_lists.h

hash_tables.o : linked_lists.h hash_tables.h

parser : hash_tables.o linked_lists.o intmdt_code_gen.o
	lex lexer.lex
	yacc parser.yacc
	gcc -g y.tab.c hash_tables.o \
		linked_lists.o intmdt_code_gen.o -o parser

clean : 
	rm -rf $(name) $(objects) lex.yy.c *.o *~ y.tab.c

