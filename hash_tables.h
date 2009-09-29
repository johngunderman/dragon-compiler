/* 
   Author: John Gunderman
*/

#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdlib.h>

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

int float_hasher (const void *key);

void hash_pretty_print_f_i (hash_table_t *table);

void hash_pretty_print_s_i (hash_table_t *table);

void hash_pretty_print_i_i (hash_table_t *table);

hash_table_t *hash_table_init (int (*cmp) (const void *, const void *), int (*hash) (const void *));

list_entry_t *hash_table_insert (hash_table_t *table, void *key, void *value);

void *hash_table_search (hash_table_t *table, void *key);

void hash_table_delete (hash_table_t *table);

#endif
