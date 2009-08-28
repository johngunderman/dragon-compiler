/* Put header info here, includes, etc. */
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/* 
   a and b are pointers to char (cast into pointers to void)
   Returns 0 if *a=*b and a non-zero value otherwise.
   TODO: Use assert (assert.c)
   TODO: use stuff from string.c
*/
int cmp_string (const void *a, const void *b) {
  if (a == NULL || b == NULL) return 1;
  return strcmp ((char *) a,(char *) b);
}


typedef struct {
  void *key;
  void *value;
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
   TODO
 */
list_head_t *list_init (int (*cmp) (const void *, const void *));


/* 
   Add a key and its corresponding value to the list.
   Does NOT check whether the key is already present in the list.
   Returns 0 on success, -1 on failure
   TODO
 */
int list_insert (list_head_t *, void *key, void *value);


/* 
   search the list for an element whose key matches the second arg.
   returns value of elem. or null if not found.
   TODO
 */
void *list_search (list_head_t *, void *key);


/* 
   De-allocates the list head as well as all of the list entries.
 */
void list_delete (list_head_t *);



//===============================//
// TESTING FUNCTIONS FOUND BELOW //
//===============================//


void cmp_string_test () {
  char word1 [] = "eecs";
  char word2 [] = "EECS";
  char word3 [] = "eec";

  assert (cmp_string (&word1, &word1) == 0);
  printf ("Passed Test: 'eecs' == 'eecs'\n");
  assert (cmp_string (&word1, &word2) != 0);
  printf ("Passed Test: 'eecs' != 'EECS'\n");
  assert (cmp_string (&word1, &word3) != 0);
  printf ("Passed Test: 'eecs' != 'eec'\n");
  assert (cmp_string (&word1, NULL) != 0);
  printf ("Passed Test: 'eecs' != NULL\n");
}


/* Test linked list with k=doubles v=doubles */
void list_doubles_test () {
  //TODO
}

/* Test linked list with k=strings v=doubles */
void list_string_doubles_test () {
  //TODO
}




//======//
// MAIN //
//======//

int main () {
  printf ("Hello World!\n");
  
  char a[] = "Hello";
  char b[] = "Hello";
  
  int ans = cmp_string (&a,&b);
  printf ("%d\n",ans);

  cmp_string_test();
  return 0;
}
