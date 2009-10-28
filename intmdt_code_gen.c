/*  
    By: John Gunderman
    This file contains function definitions for generation
    of intermediate code.
*/


#include <stdlib.h>
#include <stdio.h>


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
int gen(intmdt_code_t intermediate_code,
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

