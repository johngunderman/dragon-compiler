/* 
   Author: John Gunderman
   
   This file contains functions dealing with linked lists:
   creation, deletion, appending, searching. These linked 
   lists have key-value pairs and are meant for use as
   part of a symbol table for a C-like compiler.
*/

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

struct list_entry_t;

typedef struct { 
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
    printf ("Failed malloc in list_init");
    exit (1);
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
   Returns 0 on success, -1 on failure
   TODO
 */
int list_insert (list_head_t *head, void *key, void *value) {
  list_entry_t *entry = malloc (sizeof (list_entry_t));
  
  if (entry == NULL) {
    printf ("Failed malloc in list_insert");
    exit (1);
  }
  
  entry->key = key;
  entry->value = value;
  entry->next = NULL;
  
  if (head->list == NULL) {
    head->list = entry;
  } else {
    list_entry_t *current = head->list;
    while (current->next != NULL) {
      current = current->next;
    }
    current->next = entry;
  }
  //TODO: When should we return -1?
  return 0;
}


/* 
   search the list for an element whose key matches the second arg.
   returns value of elem. or null if not found.
   TODO
 */
void *list_search (list_head_t *head, void *key) {
  if (cmp_string (head->list->key, key) == 0) {
    return head->list->value;
  } else {
    list_entry_t *current = head->list->next;
    
    while (cmp_string (current, key) != 0) {
      if (current->next == NULL) {
	return NULL;
      } else current = current->next;
    }
    return current->value;
    //TODO!
  }
}


/* 
   De-allocates the list head as well as all of the list entries.
 */
void list_delete (list_head_t *head) {
  list_entry_t *current = head->list;
  list_entry_t *next = current->next;
  
  free (head);
  while (current != NULL) {
    free (current);
    current = next;
    next = current->next;
  }
}

