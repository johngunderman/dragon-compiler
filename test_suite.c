/* 
   Author: John Gunderman

   This is the main file for a test suite of the
   dragon-book compiler. It later will be broken
   down into separate files as the codebase grows.
 */

#include <assert.h>
#include <stdio.h>
#include "linked_lists.h"
#include "hash_tables.h"
#include "intmdt_code_gen.h"


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
  
  list_delete (head);
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

  list_delete (head);
}


//==================//
// Hash Table Tests //
//==================//

void hash_doubles_test () {
  hash_table_t *head = hash_table_init (cmp_double, double_hasher);
  double value1 = 1;
  double value2 = 2;

  hash_table_insert (head, &value1, &value2);
  double value3 = 5;
  double value4 = 6;
  hash_table_insert (head, &value3, &value4);
  double value5 = 2;
  double value6 = 9;
  hash_table_insert (head, &value5, &value6);
  
  printf ("Testing hash table with k=doubles, v=doubles:\n");
  
  double value7 = 1;
  assert (*(double*)hash_table_search (head, &value7) == 2.0);
  printf ("Passed Test 1\n");

  value7 = 5;
  assert (*(double*)hash_table_search (head, &value7) == 6.0);
  printf ("Passed Test 2\n");
  

  value7 = 2;
  assert (*(double*)hash_table_search (head, &value7) == 9);
  printf ("Passed Test 3\n");  

  value7 = 4;
  assert ((double*)hash_table_search (head,&value7 ) == NULL);
  printf ("Passed Test 4: non-valid key test\n");
  
  hash_table_delete (head);  
}

void hash_string_doubles_test () {
  hash_table_t *table = hash_table_init (cmp_string, string_hasher);
  char value1[] = "hello";
  char value2[] = "world";
  char value3[] = "foobar";
  double value4 = 23;
  double value5 = 47;
  double value6 = 91;

  char extra[] = "notakey";

  printf ("Testing hash table with k=strings v=doubles:\n");
  
  hash_table_insert (table, &value1, &value4);
  hash_table_insert (table, &value2, &value5);
  hash_table_insert (table, &value3, &value6);

  assert (*(double*)hash_table_search(table,&value1) == 23.0);
  printf ("Passed the first test.\n");
  assert (*(double*)hash_table_search(table, &value2) == 47.0);
  printf ("Passed the second test. \n");
  assert (*(double*)hash_table_search(table, &value3) == 91.0);
  printf ("Passed the third test. \n");
  assert ((double*)hash_table_search(table,&extra) == NULL);
  printf ("Passed the fourth test: bad key returns NULL.\n");
  
  hash_table_delete (table);
}


void sizeofidtype_test () {

  printf ("Testing sizeofidtype function:\n");
  
  id_type_t *temp1 = malloc (sizeof(id_type_t));
  temp1->type = &int_var;
  temp1->dimension = 0;
  temp1->size = 0;
  temp1->subsize = NULL;
  temp1->supersize = NULL;

  assert(sizeofidtype(temp1) == sizeof(int));
  printf("Passed 1st test ");
  print_id_type(temp1);
  printf("\n");
  

  id_type_t *temp2 = malloc (sizeof(id_type_t));
  temp2->type = &float_var;
  temp2->dimension = 0;
  temp2->size = 0;
  temp2->subsize = NULL;
  temp2->supersize = NULL;

  assert(sizeofidtype(temp2) == sizeof(float));
  printf("Passed 2nd test ");
  print_id_type(temp2);
  printf("\n");
  

  id_type_t *temp3 = malloc (sizeof(id_type_t));
  temp3->type = &float_var;
  temp3->dimension = 1;
  temp3->size = 10;
  temp3->subsize = temp2;
  temp2->supersize = temp3;
  temp3->supersize = NULL;
  
  assert(sizeofidtype(temp3) == 10 * sizeof(float_var));
  printf("Passed 3rd test ");
  print_id_type(temp3);
  printf("\n");
  

  id_type_t *temp4 = malloc (sizeof(id_type_t));
  temp4->type = &float_var;
  temp4->dimension = 2;
  temp4->size = 50;
  temp4->subsize = temp3;
  temp3->supersize = temp4;
  temp4->supersize = NULL;

  assert(sizeofidtype(temp4) == 50 * 10 * sizeof(float_var));
  printf("Passed 4th test ");
  print_id_type(temp4);
  printf("\n");
}

void newtemp_test() {
  env_t *env = init_env();
  intmdt_addr_t *t1 = newtemp(env);
  intmdt_addr_t *t2 = newtemp(env);
  
}

void gen_test() {

}


//======//
// MAIN //
//======//

int main () {
  printf ("\n");
  cmp_string_test();
  printf ("\n");
  list_doubles_test();
  printf ("\n");
  list_string_doubles_test();
  printf ("\n");
  hash_string_doubles_test();
  printf ("\n");
  hash_doubles_test();
  printf ("\n");
  sizeofidtype_test();
  printf ("\n");
  return 0;
}


