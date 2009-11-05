/*  
    By: John Gunderman
    This file contains function definitions for generation
    of intermediate code.

*/


#include <stdlib.h>
#include <stdio.h>
#include "intmdt_code_gen.h"

/* Constants for the type of data structures in the language. */
int int_var = 1;
int double_var = 2;
int float_var = 3;
int true_var = 4;
int false_var = 5;
int unknown_var = 0;


/* 
   Initialize the intermediate_code variable for the system.
*/
intmdt_code_t *init_code() {
  intmdt_code_t *temp = malloc( sizeof( intmdt_code_t ));
  if (temp == NULL) {
    fprintf(stderr, "Error: failed to malloc intmdt_code_t in init_code()\n");
    exit(1);
  }
  temp->n = 0;
  return temp;
}

/* 
   Initialize the environment variable for the program.
*/
env_t *init_env(hash_table_t *table) {
  env_t *temp = malloc( sizeof(env_t ));
  if (temp == NULL) {
    fprintf(stderr, "Error: failed to malloc env_t in init_env()\n");
    exit(1);
  }
  temp->prev = NULL;
  temp->table = table;
  return temp;
}

/*  
    Creates a new temporary variable and inserts it in the
    symbol table. Temp vars begin with @. Exits program on failure.
*/
intmdt_addr_t *newtemp(env_t *top) {
  static int temp_num = 0;  	/* the id for the new temp */
  
  temp_num++;

  char *key = malloc (sizeof(char) * 15); /* we assume we shouldn't need
					   more than 15 characters. */
  
  if (key == NULL) {
    fprintf(stderr, "Failed to malloc key in newtemp()\n");
    exit(1);
  }
  
  id_type_t *value = malloc (sizeof(id_type_t));

  if (value == NULL) {
    fprintf(stderr, "Failed to malloc value in newtemp()\n");
    exit(1);
  }

  value->type = &unknown_var;		/* set to 8 for unknown type */
  value->dimension = 0;
  value->size = 0;
  value->subsize = NULL;
  value->supersize = NULL;
  
  sprintf(key, "@%d", temp_num);
  
  list_entry_t *entry = hash_table_insert (top->table, key, value);
  
  intmdt_addr_t *addr = malloc ( sizeof(intmdt_addr_t));

  if (addr == NULL) {
    fprintf(stderr, "Failed to malloc intmdt_addr_t in newtemp()\n");
    exit(1);
  }

  addr->type = symbol;
  (addr->addr).entry_ptr = entry;
  
  return addr;
  
}


/*  
    This function returns the size of the type passed to it.
*/
unsigned int sizeofidtype(id_type_t *t) {
  if (t->dimension > 0) {
    return t->size * sizeofidtype(t->subsize);
  }

  switch (*(unsigned int *)t->type) {
  case 1:
    return sizeof(int);
  case 2:
    return sizeof(double);
  case 3:
    return sizeof(float);
  case 4:
    return sizeof(int); //sizeof true set to int
  case 5:
    return sizeof(int); //sizeof false set to int 
  default:
    fprintf(stderr, "Unrecognized type, could not compute size\n");
    exit(1);
  }
}




/*  
    Prints out the contents of the given intmdt_addr_t.
*/
void intmdt_addr_print(intmdt_addr_t *t) {
  switch(t->type){
  case symbol:
    printf("Symbol: %s\t", (char*) (t->addr).entry_ptr->key);
    break;
  case int_const:
    printf("Integer: %d\t", *(t->addr).int_const_ptr);
    break;
  case float_const:
    printf("Float: %f\t", *(t->addr).float_const_ptr);
    break;
  case bool_const:
    printf("Bool: %d\t", *(t->addr).bool_const_ptr);
    break;
  case code:
    printf("Code: %p\t", (void *) *(t->addr).int_const_ptr);
  }
}


/*  
    
*/
void intmdt_code_print(intmdt_code_t *code) {
  printf("Intermediate Code:\n");
  printf("Op\tArg1\t\tArg2\t\tResult\n");
  unsigned int i = 0;
  while (i < code->n) {
    printf("OP: %s\t",code->code[i]->op);
    
    intmdt_addr_print(code->code[i]->arg1);
    
    if (code->code[i]->arg2 != NULL) {
      intmdt_addr_print(code->code[i]->arg2);
    } else printf("\t\t");
    
    intmdt_addr_print(code->code[i]->result);
    
    printf("\n");
    
    i++;
  }
}



/*  
    creates a quadruple_t containing the op, arguments, and result
    passed to this function. Then, the quadruple_t is inserted into
    the next empty location in the given intmdt_code_t. Returns 0 on
    error, 1 on success.
*/
int gen(intmdt_code_t *intermediate_code,
        char *op, intmdt_addr_t *arg1, intmdt_addr_t *arg2,
        intmdt_addr_t *result) {

  quadruple_t *instr = malloc(sizeof(quadruple_t));

  if (instr == NULL) {
    fprintf(stderr, "Error on malloc in gen();\n");
    return(0);
  }
  instr->op = op;
  instr->arg1 = arg1;
  instr->arg2 = arg2;
  instr->result = result;

  if (intermediate_code->n < MAXCODELEN) {
    intermediate_code->code[intermediate_code->n] = instr;
    intermediate_code->n++;
    return(1);
  } else { 
    fprintf(stderr, "Error in gen(): overwriting or max limit hit for intmdt_code\n");
    return(0);
  }
}

/* 
   This function generates code to change types. It effectively
   is an implicit cast to a larger data size. Returns a 
   list_entry_t on success, and NULL on failure.
   
   It currently only casts to float, because we only have the number
   types for int and float. support for doubles might be added in the
   future, but adding other types looks like a pain in the ass
   considering how many functions would need to be modified.
*/
list_entry_t *widen(intmdt_code_t* code, env_t *top, intmdt_addr_t *a, id_type_t *t) {
  intmdt_addr_t *temp = newtemp(top);
  
  if (a->type == symbol 
      && ((id_type_t*)(a->addr.entry_ptr->value))->type == &float_var
      && t->type == &float_var) {
    
    gen (code, "(float)", a, NULL, temp);
    return &(*(temp->addr).entry_ptr);
  } else {
    fprintf (stderr, "An unsupported widen() operation was attempted.\n");
    return NULL;
  }
}


/*
  Print out the contents of the id_type
  of the symbol in the symbol table.
  This function is meant to be passed
  to hash_pretty_print();
*/
void print_id_type (void *id) {
  printf("{");
  switch (*(unsigned int *)((id_type_t *)id)->type) {
  case 1:
    printf("&int_var");
    break;
  case 2:
    printf("&double_var");
    break;
  case 3:
    printf("&float_var");
    break;
  case 4:
    printf("&true_var");
    break;
  case 5:
    printf("&false_var");
    break;
  default:
    printf("unknown_type");
  } 
  printf(",%d,%d,",
	 ((id_type_t *)id)->dimension,
	 ((id_type_t *)id)->size);
  if (((id_type_t *)id)->subsize == NULL) {
    printf ("NULL");
  } else {
    print_id_type (((id_type_t *)id)->subsize);
  }
  /* print out subsize here (as string) */
  /*TODO: print out our info on the type here.*/
  if (((id_type_t *)id)->supersize == NULL) {
    printf (",NULL}");
  } else {
    printf(",%p}", (void *) ((id_type_t *)id)->supersize);
  }
}
