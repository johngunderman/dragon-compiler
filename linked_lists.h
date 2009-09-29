/*
   Author: John Gunderman
*/


#ifndef LINKED_LISTS_H
#define LINKED_LISTS_H

typedef struct list_entry_t { 
  void *key;
  void *value;
  struct list_entry_t *next;
} list_entry_t;

typedef struct {
  list_entry_t *list;
  int (*cmp) (const void *, const void *); /* Comparison function */
} list_head_t;

int cmp_string (const void *a, const void *b);

int cmp_double (const void *a, const void *b);

int cmp_float (const void *a, const void *b);

void list_pretty_print_f_i (list_head_t *head);

void list_pretty_print_s_i (list_head_t *head);

void list_pretty_print_i_i (list_head_t *head);

list_head_t *list_init (int (*cmp) (const void *, const void *));

list_entry_t *list_insert (list_head_t *head, void *key, void *value);

void *list_search (list_head_t *head, void *key);

void list_delete (list_head_t *head);

#endif
