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
   This function simply mallocs an intmdt_code_t and sets its
   n value to 0.
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
   This function mallocs a new 'env_t' variable and its
   corresponding 'hash_table_t' (through hash_table_init).
*/
env_t *init_env() {
  env_t *temp = malloc( sizeof(env_t ));
  if (temp == NULL) {
    fprintf(stderr, "Error: failed to malloc env_t in init_env()\n");
    exit(1);
  }

  hash_table_t *table = hash_table_init(cmp_string, string_hasher);
  if (table == NULL) {
    fprintf(stderr, "Error: failed to malloc hash_table_t in init_env()\n");
  }

  temp->prev = NULL;
  temp->table = table;
  return temp;
}

/*  
    Creates a new temporary variable and inserts it in the
    symbol table. Temp vars begin with @. Returns NULL on failure.
*/
intmdt_addr_t *newtemp(env_t *top) {
  static int temp_num = 0;  	/* the id for the new temp */
  
  if (top == NULL || top->table == NULL) {
    fprintf(stderr, "Attempted to call newtemp with null environment.\n");
    return NULL;
  }
  
  temp_num++;

  char *key = malloc (sizeof(char) * 15); /* we assume we shouldn't need
					   more than 15 characters. */
  
  if (key == NULL) {
    fprintf(stderr, "Failed to malloc key in newtemp()\n");
    return NULL;
  }
  
  id_type_t *value = malloc (sizeof(id_type_t));

  if (value == NULL) {
    fprintf(stderr, "Failed to malloc value in newtemp()\n");
    return NULL;
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
    return NULL;
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
 
  if (t == NULL) {
    printf("\t\t");
    return;
  }
  
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
    printf("Code: %p\t", (void*) t->addr.instr_ptr);
  }
}


/*  
    
*/
void intmdt_code_print(intmdt_code_t *code) {
  printf("Intermediate Code:\n");
  printf("Op\tArg1\t\tArg2\t\tResult\n");
  unsigned int i = 0;
  while (i < code->n) {
    printf("%s\t",code->code[i]->op);
    
    intmdt_addr_print(code->code[i]->arg1);
    
    intmdt_addr_print(code->code[i]->arg2);

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
  
  if (a->type == symbol 
      && ((id_type_t*)(a->addr.entry_ptr->value))->type == &int_var
      && t->type == &float_var) {

    intmdt_addr_t *temp = newtemp(top);
    gen (code, "(float)", a, NULL, temp);
    return &(*(temp->addr).entry_ptr);
  } else {
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
  printf("{loc=%d, ", ((id_type_t*)id)->location);
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

/*  
    Generates and pushes a new symbol table onto the environment.
    Returns a pointer to the new top of the stack.
    
    NOTE: the given env_t pointer IS NOT changed
    to point to the new top of the stack. Only the
    returned pointer points to the new stack top.
*/
env_t *push_env_table(env_t *env) {
  env_t *top = init_env();
  
  top->table = hash_table_init(cmp_string ,string_hasher);
  top->prev = env;

  return top;
}

/* 
   Pops the top symbol table off the environment.
   Returns a pointer to the new top of the stack.
   
   NOTE: the given env_t pointer IS NOT changed
   to point to the new top of the stack. Only the
   returned pointer points to the new stack top.
*/
env_t *pop_env_table(env_t *env) {
  if (env == NULL) {
    fprintf(stderr, "Attempted to pop from a null environment\n");
    exit(1);
  }
  /* TODO: free our table, but we don't do that quite yet, because
   we still want the references to stuff in the table.*/
  //hash_table_delete(env->table);
  
  env_t *temp = env->prev;
  
  free (env);
  
  return temp;
  
}


/* 
   Print out a string. This function is meant to be passed to
   hash_pretty_print().
*/
void print_str (void *str) {
  printf ("%s", (char *) str);
}


void print_env(env_t *head) {
  unsigned int i = 0;
  while (head != NULL && head->table != NULL) {
    printf("Environment Table (Top - %d)\n",i);
    hash_pretty_print (head->table, print_str, print_id_type);
    head = head->prev;
  }
}

/* 
   Searches through every symbol table in the environment
   for a symbol matching the given string. Returns NULL if
   no symbol is found.
*/
id_type_t *env_search(env_t *env, char *str) {
  id_type_t *result = NULL;
  
  while (env != NULL && env->table != NULL) {
    result = hash_table_search(env->table, str);

    if (result != NULL) {
      return result;
    }
    env = env->prev;
  }
  /* Returns NULL if not found */
  return result;
}


/*  
    Creates a new list containing only instr_ptr.
    Returns a pointer to the newly created list.
    Returns NULL on failure.
    This function uses the linked_lists functions
    and structs, but passes NULL for unnecissary 
    arguments, such as cmp() and *key.
*/
list_head_t *list_makelist(quadruple_t *instr_ptr) {
  list_head_t *head =  list_init(quadruple_cmp);

  list_insert(head, NULL, instr_ptr);

  return head;
}


/*  
    Takes the list, p, of quadruples, and sets the
    result of all the quadruples to the given quadruple
    i. Result should be a pointer to the quadruple that
    the instruction will jump to.
    Returns 0 on success, 1 on failure.
*/
int backpatch(list_head_t *p, quadruple_t *i) {
  
  if (p == NULL) {
    fprintf(stderr, "list head passed to backpatch() was NULL.\n");
    return 0;
  }
  list_entry_t *current = p->list;
  
  while (current != NULL) {
    intmdt_addr_t *res = malloc(sizeof(intmdt_addr_t));
    
    if (res == NULL) {
      fprintf(stderr, "failed to malloc intmdt_addr_t in backpatch()\n");
      return 1;
    }
    
    res->type = code;
    res->addr.instr_ptr = i;
    
    ((quadruple_t*) current->value)->result = res;
    current = current->next;
  }
  return 0;
}


/*  
    Concatenates the lists pointed to by p1 and p2 and returns
    the concatenated list.
*/
list_head_t *list_merge(list_head_t *p1, list_head_t *p2) {
  
  if (p1 == NULL) {
    fprintf(stderr, "list head p1 passed to list_merge() was NULL.\n");
    return NULL;
  }
  if (p2 == NULL) {
    fprintf(stderr, "list head p2 passed to list_merge() was NULL.\n");
    return NULL;
  }

  list_entry_t *current = p1->list;
  
  /* Find the end of the first list */
  while(current->next != NULL) {
    current = current->next;
  }
  
  current->next = p2->list;
  
  free(p2);

  return p1;
}


/* 
   Does absolutely nothing. Only exists so we can
   use it as *cmp and not have list_init complain.
*/
int quadruple_cmp(const void *key, const void *value){
  return (key == value);
}


/* 
   Calculates the size of the given environment. All of it, not just
   the top environment. Everything. It just iterates over everything
   and adds it all up. it could make things slow if we use it a lot
   but atm speed is not the primary concern.
*/
unsigned int sizeofenv(env_t *env) {
  unsigned int size = 0;
  
  while (env != NULL) {
    for (unsigned int i = 0; i < env->table->size; i++) {
      if (env->table->entries[i] != NULL) {
	list_entry_t *current =  env->table->entries[i]->list;
	while(current != NULL ) {
	  size += sizeofidtype((id_type_t *) current->value);
	  current = current->next;
	}
      }
    }
    env = env->prev;
  }
  return size;
}



/* ===========================  */
/* Structure Freeing Functions  */
/* ===========================  */


void free_env(env_t *env) {
  env_t *temp;
  
  while (env != NULL && env->table != NULL) {
    temp = env->prev;
    hash_table_delete(env->table);
    free(env);
    env = temp;
  }
  env = NULL;
}


void free_quadruple(quadruple_t *quad) {
  /* if (quad->arg1 != NULL) { */
/*     free(quad->arg1); */
/*     quad->arg1 = NULL; */
/*   } */
  
/*   if (quad->arg2 != NULL) { */
/*     free(quad->arg2); */
/*     quad->arg2 = NULL; */
/*   } */
  
/*   if (quad->result != NULL) { */
/*     free(quad->result); */
/*     quad->result = NULL; */
/*   } */

  free (quad);
  quad = NULL;
}


void free_intmdt_code(intmdt_code_t *code) {
  unsigned int i = 0;
  while (i < code->n) {
    free_quadruple(code->code[i]);
    i++;
  }
  free(code);
  code = NULL;
}



void free_id_type(id_type_t *id) {
  id_type_t *temp;

  while ( id != NULL ) {
    temp = id->subsize;
    free (id);
    id = temp;
  }
  id = NULL;
}
