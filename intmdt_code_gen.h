/* 
   Header for intmdt_code_gen.c
   By: John Gunderman
   This file contains structs and function prototypes dealing with
   the generation of intermediate code.
*/

#include "hash_tables.h"

#define MAXCODELEN 8192

/* TODO: floats not entirely supported as of yet. */
typedef struct {
  enum {symbol, int_const, float_const, bool_const, code} type;
  union {
    list_entry_t *entry_ptr;
    int *int_const_ptr;
    float *float_const_ptr;
    int *bool_const_ptr;
    quadruple_t *instr_ptr;
  } addr;
} intmdt_addr_t;


typedef struct quadruple quadruple_t;
struct quadruple {
  char *op;
  intmdt_addr_t *arg1;
  intmdt_addr_t *arg2;
  intmdt_addr_t *result;
};


typedef struct {
  quadruple_t code[MAXCODELEN];
  unsigned int n; /* Number of instructions in code */
} intmdt_code_t;


typedef struct {
  hash_table_t *this;
  hash_table_t *prev;
} env_t;


int gen(intmdt_code_t *intermediate_code,
        char *op, intmdt_addr_t *arg1, intmdt_addr_t *arg2,
        intmdt_addr_t *result);

intmdt_addr_t *newtemp(env_t *top);

unsigned int sizeofidtype(id_type_t *t);

list_entry_t *widen(intmdt_addr_t *a, id_type_t *t);

