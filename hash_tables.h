/* 
   Author: John Gunderman

   This file contains functions dealing with hash tables:
   creation, deletion, inserting, searching. These hash
   tables are meant for use as part of the symbol table
   for a C-like compiler.
 */


#include "linked_lists.h"
#include <stdlib.h>
#include <assert.h>

#define TABLE_SIZE 100


typedef struct {
  size_t size;
  int (*cmp) (const void *, const void *);
  int (*hash) (const void *);
  list_head_t **entries; 	/* entries is a dynamically (not really)
				 sized array of linked lists */
} hash_table_t;


/* 
   This function hashes strings for insertion into the hash table.
 */
int string_hasher (const void *key) {
  int total = 0;
  int i = 0;

  while (*((char *) key + i) != 0){
    total += *((char *)key + i) + total * 31;
    i++;
  }
  return total;
}

/*
  Generate an integer hash of a double. currently just casts to
  an int and returns, which is probably not the "best" way, but
  it certainly works fine.
 */
int double_hasher (const void *key) {
  return (int) *(double *)key;
}

/*
  Pretty prints the hash table for K:float V:int
*/
hash_pretty_print_f_i (hash_table_t *table){
  int i = 0;
  while ( i < table->size) {
    if (table->entries[i] != NULL) {
      list_pretty_print_f_i(table->entries[i]);
    }
    i++;
  }
}


/*
  Pretty prints the hash table for K:string V:int
*/  
hash_pretty_print_s_i (hash_table_t *table){
  int i = 0;
  while ( i < table->size) {
    if (table->entries[i] != NULL) {
      list_pretty_print_s_i(table->entries[i]);
    }
    i++;
  }
}



/* 
   Allocates a new table with default size 100,
   assigns the comparison function,
   and returns an empty table. On error return NULL.
   Takes a comparison function and a hashing function.
 */
hash_table_t *hash_table_init (int (*cmp) (const void *, const void *), int (*hash) (const void *)) {
  hash_table_t *table = malloc (sizeof (hash_table_t));
  
  if (table == NULL) {
    fprintf (stderr, "Malloc failed in hash_table_init");
    return NULL;
  }

  table->cmp = cmp;
  table->hash = hash;
  table->entries = calloc (TABLE_SIZE, sizeof (list_head_t*));
  table->size = TABLE_SIZE;
  
  return table;
}

/* 
   Add a key and its corresponding value to the table.
   Does NOT check if the key is already present.
   Does NOT overwrite keys, but previous key may become unreachable.
   Returns pointer to the entry on success, NULL on failure.
*/
list_entry_t *hash_table_insert (hash_table_t *table, void *key, void *value) {
  int hash = table->hash (key) % table->size;
  
  if (table->entries[hash] == NULL) {
    table->entries[hash] = list_init (table->cmp);
  }
  return list_insert (table->entries[hash], key, value);
}


/* 
   Search the table for an element whose key matches the second arg.
   returns value of elem or NULL if not found.
*/
void *hash_table_search (hash_table_t *table, void *key) {
  int hash = table->hash (key) % table->size;
  
  if (table->entries[hash] == NULL) return NULL;
  else return list_search (table->entries[hash], key);
}


/* 
   De-allocates the table as well of all of its entries.
 */
void hash_table_delete (hash_table_t *table) {
  size_t i = 0;
  while ( i < table->size) {
    if (table->entries[i] != NULL) {
      list_delete (table->entries[i]);
    }
    i++;
  }
  free (table);
}
