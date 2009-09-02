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

  printf ("Testing string comparison function:\n");

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
  
  printf ("Testing linked list with k=doubles, v=doubles:\n");
  
  double value7 = 1;
  assert (*(double*)list_search (head, &value7) == 2.0);
  printf ("Passed Test 1\n");

  value7 = 5;
  assert (*(double*)list_search (head, &value7) == 6.0);
  printf ("Passed Test 2\n");
  

  value7 = 2;
  assert (*(double*)list_search (head, &value7) == 9);
  printf ("Passed Test 3\n");  

  value7 = 4;
  assert ((double*)list_search (head,&value7 ) == NULL);
  printf ("Passed Test 4: non-valid key test\n");
}

/* test linked list with k=strings v=doubles */
void list_string_doubles_test () {
  list_head_t *head = list_init (cmp_string);
  char value1[] = "hello";
  char value2[] = "world";
  char value3[] = "foobar";
  double value4 = 23;
  double value5 = 47;
  double value6 = 91;

  char extra[] = "notakey";

  printf ("Testing linked list with k=strings v=doubles:\n");
  
  list_insert (head, &value1, &value4);
  list_insert (head, &value2, &value5);
  list_insert (head, &value3, &value6);

  assert (*(double*)list_search(head,&value1) == 23.0);
  printf ("Passed the first test.\n");
  assert (*(double*)list_search(head, &value2) == 47.0);
  printf ("Passed the second test. \n");
  assert (*(double*)list_search(head, &value3) == 91.0);
  printf ("Passed the third test. \n");
  assert ((double*)list_search(head,&extra) == NULL);
  printf ("Passed the fourth test: bad key returns NULL.\n");

  //TODO
}




//======//
// MAIN //
//======//

int main () {
  cmp_string_test();
  printf ("\n");
  list_doubles_test();
  printf ("\n");
  list_string_doubles_test();
  return 0;
}
