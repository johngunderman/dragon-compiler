/*  
    By: John Gunderman
    This file contains function definitions for generation
    of intermediate code.
*/


#include <stdlib.h>
#include <stdio.h>

/*  
    Creates a new temporary variable and inserts it in the
    symbol table. Temp vars begin with @. Exits program on failure.
*/
intmdt_addr_t *newtemp(env_t *top) {
  static int temp_num = 0;  	/* the id for the new temp */
  
  char *key = malloc (sizeof(char * 15)); /* we assume we shouldn't need
					   more than 15 characters. */
  
  if (key == NULL) {
    fprintf(stderr, "Failed to malloc key in newtemp()");
    exit(1);
  }
  
  id_type_t *value = malloc (sizeof(id_type_t));

  if (value == NULL) {
    fprintf(stderr, "Failed to malloc value in newtemp()");
    exit(1);
  }

  value->type = NULL;
  value->dimension = 0;
  value->size = 0;
  value->subsize = NULL;
  value->supersize = NULL;
  
  sprintf(key, "@%d", temp_num);
  
  list_entry_t *entry = hash_table_insert (top->table, key, value);
  
  intmdt_addr_t *addr = malloc ( sizeof(intmdt_addr_t));

  if (intmdt_addr_t == NULL) {
    fprintf(stderr, "Failed to malloc intmdt_addr_t in newtemp()");
    exit(1);
  }

  addr->type = symbol;
  addr->entry_ptr = entry;
  
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
void intmdt_code_print(intmdt_addr_t *t) {
  switch(t->type){
  case symbol:
    printf("Symbol: %s\n", t->addr->entry_ptr->key); 
    break;
  case int_const:
    printf("Integer: %d\n", *t->addr->int_const_ptr);
    break;
  case float_const:
    printf("Float: %f\n", *t->addr->float_const_ptr);
    break;
  case bool_const:
    printf("Bool: %d\n", *t->addr->bool_const_ptr);
    break;
  case code:
    printf("Code: &p\n", *t->addr->int_const_ptr);
    
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

  if (*instr == NULL) {
    fprintf(stderr, "Error on malloc in gen();");
    return(0);
  }
  instr->op = op;
  instr->arg1 = arg1;
  instr->arg2 = arg2;
  instr->result = result;

  if (intermediate_code[n] == NULL && n < MAXCODELEN) {
    intermediate_code[n] = instr;
    n++;
    return(1);
  } else { 
    fprintf(stderr, "Error in gen(): overwriting or max limit hit for intmdt_code");
    return(0);
}

