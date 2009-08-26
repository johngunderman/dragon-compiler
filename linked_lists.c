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







