/* 
   Author: John Gunderman

   This file contains functions dealing with hash tables:
   creation, deletion, inserting, searching. These hash
   tables are meant for use as part of the symbol table
   for a C-like compiler.
 */


#include "linked_lists.h"
#include <stdlib.h>

#define TABLE_SIZE 100


typedef struct {
  size_t size;
  int (*cmp) (const void *, const void *);
  int (*hash) (const void *);
  list_head_t **entries; 	/* entries is a dynamically
				 sized array of linked lists */
} hash_table_t;


/* 
   This function hashes strings for insertion into the hash table.
 */
int string_hasher (const void *key) {
  int total = 0;
  int i = 0;
  while (((char *) key + i) != NULL){
    total += *((char *)key + i);
    i++;
  }
  return total;
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
  table->entries =  calloc (TABLE_SIZE, sizeof (list_head_t*));
  table->size = TABLE_SIZE;
  
  return table;
}

/* 
   Add a key and its corresponding value to the table.
   Does NOT check if the key is already present.
   Does NOT overwrite keys, but previous key may become unreachable.
   
   TODO: figure out what the hell the behavior of this function
   should be.
   
   Returns 0 on success, -1 on failure.
*/
int hash_table_insert (hash_table_t *table, void *key, void *value) {
  int hash = table->hash (key) % table->size;
  
  list_entry_t *entry = malloc (sizeof (list_entry_t));
  if (entry == NULL) {
    fprintf (stderr, "Error on malloc in hash_table_insert");
    return -1;
  }

  if (table->entries[hash] == NULL) {
    table->entries[hash] = list_init (table->cmp);
  }
  list_insert (table->entries[hash], key, value);
  return 0;
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
void hash_table_delete (hash_table_t *table);
