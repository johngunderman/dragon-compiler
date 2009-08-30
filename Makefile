
objects = test_suite.o
name = linked_lists

#dragon-compiler : $(objects)
# 	gcc -o $(name) $(objects)

test : $(objects)
	gcc -o test_suite $(objects)

test_suite.o : linked_lists.h

clean : 
	rm $(name) $(objects)