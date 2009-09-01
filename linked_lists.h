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



/* TESTING: */
void pretty_print (list_head_t *head) {
  list_entry_t *current = head->list;
  while (current != NULL) {
    printf ("%f, %f, --> ",*(double*)current->key, *(double*)current->value );
    current = current->next;
  }
  printf ("\n");
}


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
    fprintf (stderr,"Failed malloc in list_insert");
    return -1;
  }
  
  entry->key = key;
  entry->value = value;
  
  entry->next = head->list;
  head->list = entry;
  

  pretty_print (head);

  return 0;
}


/* 
   search the list for an element whose key matches the second arg.
   returns value of elem. or null if not found.
   TODO
 */
void *list_search (list_head_t *head, void *key) {
  printf ("Entering list_search\n");
  
  if (head->cmp (head->list->key, key) == 0) {
    printf ("list key is %f and key is %f\n",*(double*)head->list->key, *(double*)key);
    printf ("the value is %f", *(double*)head->list->value);
    printf ("1\n");
    return head->list->value;
  } else {
    printf ("2\n");
    list_entry_t *current = head->list->next;
    
    
    printf ("current is %f\n", *(double*)(current->value));     
    printf ("cmp to %f is %d\n",*(double*)key, head->cmp (current, key));

    while (head->cmp (current, key) != 0) {
      printf ("3\n");
      printf ("%f\n", *(double*)current->next->key);
      if (current->next == NULL) {
	printf ("Oh no! we have a null!");
	return NULL;
      } else {
	printf ("foobar");
	current = current->next;
      }
    }
    printf ("4\n");
    printf ("returned value is %f\n",*(double*)(current->value));
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
