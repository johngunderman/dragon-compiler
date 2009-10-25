/* 
   Author: John Gunderman
*/

#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdlib.h>
#include "linked_lists.h"

#define TABLE_SIZE 100


typedef struct {
  size_t size;
  int (*cmp) (const void *, const void *);
  int (*hash) (const void *);
  list_head_t **entries; 	/* entries is a dynamically (not really)
				 sized array of linked lists */
} hash_table_t;

int string_hasher (const void *key);

int double_hasher (const void *key);

void hash_pretty_print (hash_table_t *table, void (*keyp) (void *),
			void (*valuep) (void *));

hash_table_t *hash_table_init (int (*cmp) (const void *, const void *), int (*hash) (const void *));

list_entry_t *hash_table_insert (hash_table_t *table, void *key, void *value);

void *hash_table_search (hash_table_t *table, void *key);

void hash_table_delete (hash_table_t *table);

#endif
