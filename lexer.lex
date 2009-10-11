%{
  
  /* 
     Author: John Gunderman
     
     This file contains a lexical analyzer for
     a C-like compiler.
   */
  
  #include <stdio.h>
  #include <string.h>
  #include "hash_tables.h"  

   void *yyval;
  hash_table_t *sym_table; 
  hash_table_t *num_table;
  hash_table_t *real_table;

  int int_var = 1;
  int double_var = 2;

  int true_var = 3;
  int false_var = 4;


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
   Similar to install_num, but stores doubles instead of int.
*/
void *install_real() {
  double *real = malloc (sizeof (double));
  *real = atof(yytext);
  if (!hash_table_search(real_table, real)) {
    return hash_table_insert(real_table, real, &double_var);
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

{ws}     {/* no action and no return */}
if       {yyval = NULL; return(IF);}
do       {yyval = NULL; return(DO);}
while    {yyval = NULL; return(WHILE);}
break    {yyval = NULL; return(BREAK);}
then     {yyval = NULL; return(THEN);}
else     {yyval = NULL; return(ELSE);}
true     {yyval = &true_var; return(TRUE);}
false    {yyval = &false_var; return(FALSE);}
int	 {yyval = (void *)&int_var; return(BASIC);}
double	 {yyval = (void *)&double_var; return(BASIC);}
{id}     {yyval = (void *) install_id(); return(ID);}
{number} {yyval = (void *) install_num(); return (NUM);}
{real}   {yyval = (void *) install_real(); return (REAL);}
\<       {yyval = NULL; return(LT);}
>        {yyval = NULL; return(GT);}
\<=      {yyval = NULL; return(LE);}
>=       {yyval = NULL; return(GE);}
==       {yyval = NULL; return(EQ);}
!=       {yyval = NULL; return(NE);}
\n|.     {yyval = NULL; return yytext[0];}

%%



