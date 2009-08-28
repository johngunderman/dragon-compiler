
objects = linked_lists.o
name = linked_lists

dragon-compiler : $(objects)
	gcc -o $(name) $(objects)

clean : 
	rm $(name) $(objects)