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
  list_head_t *head = list_init (cmp_string);
  double value1 = 1;
  double value2 = 2;

  list_insert (head, &value1, &value2);
  value1 = 5;
  value2 = 6;
  list_insert (head, &value1, &value2);
  value1 = 2;
  value2 = 9;
  list_insert (head, &value1, &value2);

  value1 = 1;
  printf ("Value of list_search: %d", *(double*)list_search (head, &value1));

  assert (*(double*)list_search (head, &value1) == 6);
  printf ("Passed Test\n");

  value1 = 5;
  assert (*(double*)list_search (head, &value1) == 6);
  printf ("Passed Test 2\n");
  

  value1 = 2;
  assert (*(double*)list_search (head, &value1) == 6);
  printf ("Passed Test 3\n");  

  value1 = 4;
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
