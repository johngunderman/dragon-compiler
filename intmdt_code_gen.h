/* 
   Header for intmdt_code_gen.c
   By: John Gunderman
   This file contains structs and function prototypes dealing with
   the generation of intermediate code.
*/

#include "hash_tables.h"
#include "linked_lists.h"

#define MAXCODELEN 8192


/* Constants for the type of data structures in the language. */
int int_var;
int double_var;
int float_var;
int true_var;
int false_var;
int unknown_var;


typedef struct {
        list_head_t *truelist;
        list_head_t *falselist;
} boolean_list_t;


typedef struct quadruple quadruple_t;

/* TODO: floats not entirely supported as of yet. */
typedef struct {
  enum {symbol, int_const, float_const, bool_const, code} type;
  union {
    struct list_entry_t *entry_ptr;
    int *int_const_ptr;
    float *float_const_ptr;
    int *bool_const_ptr;
    quadruple_t *instr_ptr;
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


typedef struct env_t {
  hash_table_t *table;
  struct env_t *prev;
} env_t;


typedef struct id_type_t {
  void *type;			/* type of var */
  unsigned int dimension;	/* vector dimension */
  unsigned int size;		/* size of child dimension */
  struct id_type_t *subsize; 	/* struct of child dimensions */
  struct id_type_t *supersize; /* struct of the parent dimensions */
  unsigned int location;
} id_type_t;



int gen(intmdt_code_t *intermediate_code,
        char *op, intmdt_addr_t *arg1, intmdt_addr_t *arg2,
        intmdt_addr_t *result);

intmdt_addr_t *newtemp(env_t *top);

unsigned int sizeofidtype(id_type_t *t);

list_entry_t *widen(intmdt_code_t *code, env_t *top, intmdt_addr_t *a, id_type_t *t);

void print_id_type (void *id);

intmdt_code_t *init_code();

env_t *init_env();

void print_intmdt_code(intmdt_code_t *code);

env_t *push_env_table(env_t *env);

env_t *pop_env_table(env_t *env);

void print_env(env_t *head);

id_type_t *env_search(env_t *env, char *str);

unsigned int sizeofenv(env_t *env);

/*===================================*/
/* Truelist and Falselist Functions: */
/*===================================*/

int quadruple_cmp(const void *, const void *);

list_head_t *list_makelist(quadruple_t *instr_ptr);

list_head_t *list_merge(list_head_t *p1, list_head_t *p2);

int backpatch(list_head_t *p, quadruple_t *i);



/* ===========================  */
/* Structure Freeing Functions  */
/* ===========================  */


void free_env(env_t *env);

void free_quadruple(quadruple_t *quad);

void free_intmdt_code(intmdt_code_t *code);

void free_id_type(id_type_t *id);
