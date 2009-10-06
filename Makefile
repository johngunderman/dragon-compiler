
objects = test_suite.o linked_lists.o hash_tables.o
name = linked_lists

CFLAGS = -Wall -Wextra -Werror -pedantic -O -g -std=c99

#default : $(objects)
#	gcc -o $(name) $(objects)

test : $(objects)
	gcc -o test_suite $(objects)

test_suite.o : linked_lists.h hash_tables.h

linked_lists.o : linked_lists.h

hash_tables.o : linked_lists.h hash_tables.h

clean : 
	rm -rf $(name) $(objects) lex.yy.c *.o *~

lexer : hash_tables.o linked_lists.o
	lex lexer.lex
	gcc -c lex.yy.c
	gcc -o lexer lex.yy.o hash_tables.o linked_lists.o
