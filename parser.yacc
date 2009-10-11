/* 
   Author: John Gunderman
*/

%{
  #include <stdio.h>
  
#define YYSTYPE void *

  int yylex (void);
  void yyerror (char const *);
%}


/* 
   %expect tells bison/yacc that the 'dangling else' it
   finds in this grammar is harmless. The default action
   is to shift, and this is what we want, so we don't have
   to worry about it.
*/
%expect 1

%token NUM
%token BASIC
%token IF
%token THEN
%token ELSE
%token DO
%token WHILE
%token BREAK
%token ID
%token REAL
%token TRUE
%token FALSE
%token LT
%token GT
%token GE
%token LE
%token NE
%token EQ


%start program

%% /* Grammar rules and actions follow.  */


program : block
	;

block : '{' decls stmts '}' {printf("block->decls stmts\n");}
      ;

decls : decls decl          {printf("decls->decls decl\n");}
      | /* empty */         {printf("decls->empty\n");}
      ;

decl : type ID ';'          {printf("decl-> type ID\n");}
     ;

type : type '[' NUM ']'     {printf("type-> type [ NUM ]\n");}
     | BASIC                {printf("type-> BASIC\n");}
     ;

stmts : stmts stmt          {printf("stmts->stmts stmt\n");}
      | /* empty */         {printf("stmts->empty\n");}
      ;

stmt : loc '=' bool ';'                 {printf("stmt->loc = bool\n");}
     | IF '(' bool ')' stmt             {printf("stmt->IF ( bool ) stmt\n");}
     | IF '(' bool ')' stmt ELSE stmt   {printf("stmt->IF ( bool ) stmst ELSE stmt\n");}
     | WHILE '(' bool ')' stmt          {printf("stmt->WHILE ( bool ) stmt\n");}
     | DO stmt WHILE '(' bool ')' ';'   {printf("stmt->DO stmt WHILE ( bool ) ;\n");}
     | BREAK ';'                        {printf("stmt->BREAK ;\n");}
     | block                            {printf("stmt->block\n");}
     ;

loc : loc '[' bool ']'     {printf("loc-> loc [ bool ]\n");}
    | ID                   {printf("loc->ID\n");}
    ;

bool : bool '||' join      {printf("bool->bool || join\n");}
     | join                {printf("bool->join\n");}
     ;

join : join '&&' equality  {printf("join->join && equality\n");}
     | equality            {printf("join->equality\n");}
     ;

equality : equality EQ rel      {printf("equality->equality == rel\n");}
	 | equality NE rel      {printf("equality->equality != rel\n");}
	 | rel                  {printf("equality->rel\n");}
	 ;

rel : expr LT expr         {printf("rel->expr < expr\n");}
    | expr LE expr         {printf("rel->expr <= expr\n");}
    | expr GE expr         {printf("rel->expr >= expr\n");}
    | expr GT expr         {printf("rel->expr > expr\n");}
    | expr                 {printf("rel->expr\n");}
    ;

expr : expr '+' term       {printf("expr->expr + expr\n");}
     | expr '-' term       {printf("expr->expr - expr\n");}
     | term                {printf("expr->term\n");}
     ;

term : term '*' urnary     {printf("term->term * urnary\n");}
     | term '/' urnary     {printf("term->term / urnary\n");}
     | urnary              {printf("term->urnary\n");}
     ;

urnary : '!' urnary        {printf("urnary-> ! urnary\n");}
       | '-' urnary        {printf("urnary-> - urnary\n");}
       | factor            {printf("urnary->factor\n");}
       ;

factor : '(' bool ')'      {printf("factor->( bool )\n");}
       | loc               {printf("factor->loc\n");}
       | NUM               {printf("factor->NUM\n");}
       | REAL              {printf("factor->REAL\n");}
       | TRUE              {printf("factor->TRUE\n");}
       | FALSE             {printf("factor->FALSE\n");}
       ;



%%

#include "lex.yy.c"

void yyerror (char const * s) {
  fprintf (stderr, "%s\n", s);
  hash_pretty_print_s_i(sym_table);
}



int main () {
  yyin = stdin;

  sym_table = hash_table_init(cmp_string,string_hasher);
  num_table = hash_table_init(cmp_double,double_hasher);
  real_table = hash_table_init(cmp_double,double_hasher);

  yyparse();
   
}
