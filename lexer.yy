%{
  
  /* 
     Author: John Gunderman
     
     This file contains a lexical analyzer for
     a C-like compiler.
   */
  
  #include <stdio.h>
  #include <string.h>
  #include "hash_tables.h"  

  /*
    LT, LE, EQ, NE, GT, GE,
    IF, THEN, ELSE, ID, NUMBER, RELOP
  */

  #define LT            1
  #define LE            2
  #define EQ            3
  #define NE            4
  #define GT            5
  #define GE            6
  #define IF            7
  #define THEN          8
  #define ELSE          9
  #define ID            10
  #define NUMBER        11
  #define RELOP         12
  #define BASIC         13
  #define WHILE         14
  #define TRUE          15
  #define FALSE         16
  #define REAL          17
  #define EXIT          18
  
  void *yyval;
  hash_table_t *sym_table; 
  hash_table_t *num_table;
  hash_table_t *real_table;

  int int_var = 1;
  int float_var = 2;


/*
    Function to install the lexeme, whose first character
    is pointed to by yytext, and whose length is yyleng,
    into the symbol table and return a pointer thereto.
*/
void *install_id () {
  if (!hash_table_search(sym_table, yytext)) {
    //TODO: at some point we will be storing something besides
    // simply a pointer to int_var as the value of the pair.

    // In other news, lex is retarded and decided to use the same
    // pointer for all the data it stores in yytext.
    // The result? I overwrite all my data in hash table.
    // so we need to copy over our string before storing it
    // to make sure that we dont screw with anything after
    // the fact. Pointers are so useful, and make it so
    // easy to shoot myself in the foot... ah well.
    char *tempstr = malloc (sizeof(char) + strlen(yytext));
    strcpy (tempstr, yytext);
    return hash_table_insert(sym_table, (void *)tempstr, &int_var);
  }
  return NULL;
}


/*
    Similar to install_id, but puts numerical constants (ints) into
    a separate table.
*/
void *install_num(){
  int *num = malloc(sizeof (int));
  *num = (int) atof(yytext);
  if (!hash_table_search(num_table, num)) {
    return hash_table_insert(num_table, num, &int_var);
  }
  return NULL;
}

/* 
   Similar to install_num, but stores floats instead of int.
*/
void *install_real() {
  float *real = malloc (sizeof (float));
  *real = atof(yytext);
  if (!hash_table_search(real_table, real)) {
    return hash_table_insert(real_table, real, &float_var);
  }
  return NULL;
}

/* 
   if yywrap returns 1, the parser will terminate when
   it reaches EOF
*/
int yywrap (void) {return 1;}

%}

/* Regular Definitions */
delim   [ \t\n]
ws      {delim}+
letter  [A-Za-z]
digit   [0-9]
id      {letter}({letter}|{digit})*
number  {digit}+(E[+-]?{digit}+)?
real    {digit}+(\.{digit}+)?(E[+-]?{digit}+)?


%%

exit     {return(EXIT); /* for testing only */}

{ws}     {/* no action and no return */}
if       {yyval = NULL; return(IF);}
while    {yyval = NULL; return(WHILE);}
then     {yyval = NULL; return(THEN);}
else     {yyval = NULL; return(ELSE);}
true     {yyval = NULL; return(TRUE);}
false    {yyval = NULL; return(FALSE);}
int	 {yyval = (void *)&int_var; return(BASIC);}
float	 {yyval = (void *)&float_var; return(BASIC);}
{id}     {yyval = (void *) install_id(); return(ID);}
{number} {yyval = (void *) install_num(); return (NUMBER);}
{real}   {yyval = (void *) install_real(); return (REAL);}

%%



/* 
   A test main function so I can mess around a bit with the
   parser.
*/
int main () {
  yyin = stdin;

  sym_table = hash_table_init(cmp_string,string_hasher);
  num_table = hash_table_init(cmp_double,double_hasher);
  real_table = hash_table_init(cmp_float,float_hasher);

  while (1){
    switch (yylex()) {
    case ID:
      printf (" id ");
      break;
    case NUMBER:
      printf (" num ");
      break;
    case REAL:
      printf (" real ");
      break;
    case BASIC:
      printf (" basic ");
      break;
    case WHILE:
      printf (" while ");
      break;
    case TRUE:
      printf (" true ");
      break;
    case FALSE: 
      printf (" false ");
      break;
    case EXIT:
      goto print_on_exit;
      break;
    default:
      break;
    }
    
    
  }
 print_on_exit:
  printf ("\n\nID Table:\n");
  hash_pretty_print_s_i(sym_table);
  printf ("\n\nNUM Table:\n");
  hash_pretty_print_i_i(num_table);
  printf ("\n\nREAL Table:\n");
  hash_pretty_print_f_i(real_table);
  printf ("\n");
  
}
