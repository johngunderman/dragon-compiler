/* Put header info here, includes, etc. */


/* 
   a and b are pointers to char (cast into pointers to void)
   Returns 0 if *a=*b and a non-zero value otherwise.
   TODO: Use assert (assert.c)
   TODO: use stuff from string.c
*/
int cmp_string (const void *a, const void *b);


int cmp_string_test () {
  cmp_string ("eecs", "eecs");	/* should return 0 */
  cmp_string ("eecs", "EECS"); 	/* should return non-zero */
  cmp_string ("eecs", "eec"); 	/* non-zero */
  cmp_string ("eecs", NULL); 	/* Error handling */
}


typedef struct {
  void *key;
  void *value;
} list_entry_t;

typedef struct {
  list_entry_t *list
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
