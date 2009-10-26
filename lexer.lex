%{
  
  /* 
     Author: John Gunderman
     
     This file contains a lexical analyzer for
     a C-like compiler.
   */
  
  #include <stdio.h>
  #include <string.h>
  #include "hash_tables.h"  

  hash_table_t *sym_table; 
  




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
if       {return(IF);}
do       {return(DO);}
while    {return(WHILE);}
break    {return(BREAK);}
then     {return(THEN);}
else     {return(ELSE);}
true     {yylval = &true_var; return(TRUE);}
false    {yylval = &false_var; return(FALSE);}
int	 {yylval = (void *)&int_var; return(BASIC);}
double	 {yylval = (void *)&double_var; return(BASIC);}
float    {yylval = (void *)&float_var; return(BASIC);}
{id}     {yylval = malloc(sizeof(yytext)); strcpy(yylval, yytext); return(ID);}
{number} {int *a = malloc(sizeof(int)); *a = (int) atof(yytext);
          yylval = a; return (NUM);}
{real}   {double *a = malloc(sizeof(double)); *a = (double) atof(yytext);
          yylval = a; return (REAL);}
\<       {return(LT);}
>        {return(GT);}
\<=      {return(LE);}
>=       {return(GE);}
==       {return(EQ);}
!=       {return(NE);}
&&       {return(AND);}
\|\|     {return(OR);}
\n|.     {return yytext[0];}

%%



