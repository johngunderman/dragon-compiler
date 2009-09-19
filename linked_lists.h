/* 
   Author: John Gunderman
   
   This file contains functions dealing with linked lists:
   creation, deletion, appending, searching. These linked 
   lists have key-value pairs and are meant for use as
   part of a symbol table for a C-like compiler.
*/

#ifndef LINKED_LISTS_H
#define LINKED_LISTS_H

#include <string.h>
#include <stdlib.h>

/* 
   a and b are pointers to char (cast into pointers to void)
   Returns 0 if *a=*b and a non-zero value otherwise.
*/
int cmp_string (const void *a, const void *b) {
  if (a == NULL || b == NULL) return 1;
  return strcmp ((char *) a,(char *) b);
}

/* 
   returns 0 if a == b and 1 if a != b
   sort of redundant, but necissary in order to get a function
   pointer for the linked lists.
 */
int cmp_double (const void *a, const void *b) {
  return (*(double*)a != *(double*)b);
}


//struct list_entry_t;

typedef struct list_entry_t { 
  void *key;
  void *value;
  struct list_entry_t *next;
} list_entry_t;

typedef struct {
  list_entry_t *list;
  int (*cmp) (const void *, const void *); /* Comparison function */
} list_head_t;



/* 
   Allocates a list head,
   assigns the comparison function,
   returns the list head.
   new list should be empty
   on error return NULL.
 */
list_head_t *list_init (int (*cmp) (const void *, const void *)) {
  list_head_t *head = malloc (sizeof (list_head_t));
  
  if (head == NULL) {
    fprintf (stderr,"Failed malloc in list_init");
    return NULL;
  }

  assert (cmp != NULL);
  if (cmp == NULL) return NULL;
  
  head->cmp = cmp;
  head->list = NULL;
  return head;
}


/* 
   Add a key and its corresponding value to the list.
   Does NOT check whether the key is already present in the list.
   Returns a pointer to the new entry on success, NULL on failure
*/
list_entry_t *list_insert (list_head_t *head, void *key, void *value) {
  list_entry_t *entry = malloc (sizeof (list_entry_t));
  
  if (entry == NULL) {
    fprintf (stderr,"Failed malloc in list_insert");
    return NULL;
  }
  
  entry->key = key;
  entry->value = value;
  
  entry->next = head->list;
  head->list = entry;
  

  return entry;
}


/* 
   search the list for an element whose key matches the second arg.
   returns value of elem. or null if not found.
 */
void *list_search (list_head_t *head, void *key) {
  list_entry_t *current = head->list;
  while (current != NULL) {
    if (head->cmp (current->key, key) == 0) {
      return current->value;
    } else {
      current = current->next;
    }
  }
  return NULL;
}


/* 
   De-allocates the list head as well as all of the list entries.
 */
void list_delete (list_head_t *head) {
  list_entry_t *current = head->list;
  list_entry_t *next;

  free (head);
      
  while (current != NULL) {
    next = current->next;
    free (current);
    current = next;
  }
}

#endif
