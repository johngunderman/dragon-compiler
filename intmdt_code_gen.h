/* 
   Header for intmdt_code_gen.c
   By: John Gunderman
   This file contains structs and function prototypes dealing with
   the generation of intermediate code.
*/

#include "hash_tables.h"

#define MAXCODELEN 8192


/* Constants for the type of data structures in the language. */
int int_var;
int double_var;
int float_var;
int true_var;
int false_var;
int unknown_var;

typedef struct quadruple quadruple_t;

/* TODO: floats not entirely supported as of yet. */
typedef struct {
  enum {symbol, int_const, float_const, bool_const, code} type;
  union {
    struct list_entry_t *entry_ptr;
    int *int_const_ptr;
    float *float_const_ptr;
    int *bool_const_ptr;
    struct quadruple_t *instr_ptr;
  } addr;
} intmdt_addr_t;



struct quadruple {
  char *op;
  intmdt_addr_t *arg1;
  intmdt_addr_t *arg2;
  intmdt_addr_t *result;
};


typedef struct {
  quadruple_t *code[MAXCODELEN];
  unsigned int n; /* Number of instructions in code */
} intmdt_code_t;


typedef struct {
  hash_table_t *table;
  hash_table_t *prev;
} env_t;


typedef struct id_type_t {
  void *type;			/* type of var */
  unsigned int dimension;	/* vector dimension */
  unsigned int size;		/* size of child dimension */
  struct id_type_t *subsize; 	/* struct of child dimensions */
  struct id_type_t *supersize; /* struct of the parent dimensions */
} id_type_t;



intmdt_code_t *intermediate_code;
env_t *env;



int gen(intmdt_code_t *intermediate_code,
        char *op, intmdt_addr_t *arg1, intmdt_addr_t *arg2,
        intmdt_addr_t *result);

intmdt_addr_t *newtemp(env_t *top);

unsigned int sizeofidtype(id_type_t *t);

list_entry_t *widen(env_t *top, intmdt_addr_t *a, id_type_t *t);

void print_id_type (void *id);

intmdt_code_t *init_code();

env_t *init_env();

void print_intmdt_code(intmdt_code_t *code);
