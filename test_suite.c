/* 
   Author: John Gunderman

   This is the main file for a test suite of the
   dragon-book compiler. It later will be broken
   down into separate files as the codebase grows.
 */

#include <assert.h>
#include <stdio.h>
#include "linked_lists.h"

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
  list_head_t *head = list_init (cmp_double);
  double value1 = 1;
  double value2 = 2;

  list_insert (head, &value1, &value2);
  double value3 = 5;
  double value4 = 6;
  list_insert (head, &value3, &value4);
  double value5 = 2;
  double value6 = 9;
  list_insert (head, &value5, &value6);
  
  pretty_print (head);

  double value7 = 1;
  printf ("does 1 equal 9? %d\n", cmp_double (&value1, &value2));
  printf ("Value of list_search: %f\n", *(double*)list_search (head, &value1));

  assert (*(double*)list_search (head, &value1) == 2.0);
  printf ("Passed Test\n");

  value7 = 5;
  assert (*(double*)list_search (head, &value1) == 6);
  printf ("Passed Test 2\n");
  

  value7 = 2;
  assert (*(double*)list_search (head, &value1) == 6);
  printf ("Passed Test 3\n");  

  value7 = 4;
  assert ((double*)list_search (head,&value1 ) == NULL);
  printf ("Passed Test 4: non-valid key test\n");
}

/* test linked list with k=strings v=doubles */
void list_string_doubles_test () {
  //TODO
}




//======//
// MAIN //
//======//

int main () {
  cmp_string_test();
  printf ("\n");
  list_doubles_test();

  return 0;
}
