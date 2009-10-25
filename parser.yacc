/* 
   Author: John Gunderman
*/

%{
  #include <stdio.h>
  #include <assert.h>
  
  #define YYSTYPE void *
  
  int yylex (void);

  void yyerror (char const *);

    
  typedef struct id_type_t {
    void *type;			/* type of var */
    unsigned int dimension;	/* vector dimension */
    unsigned int size;		/* size of child dimension */
    struct id_type_t *subsize; 	/* struct of child dimensions */
    struct id_type_t *supersize; /* struct of the parent dimensions */
  } id_type_t;

  void *install_id (char *, struct id_type_t *);




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
%token AND
%token OR


 //%start program

%% /* Grammar rules and actions follow.  */


program : block
	;

block : '{' decls stmts '}' {printf("block->decls stmts\n");}
      ;

decls : decls decl          {printf("decls->decls decl\n");}
      | /* empty */         {printf("decls->empty\n");}
      ;

decl : type ID ';'          {printf("decl-> type ID\n");
                             install_id((char*)$2, (id_type_t*)$1); 
			    }
     ; 	     

type : type '[' NUM ']'     {printf("type-> type [ NUM ]\n");
                             $$ =  malloc(sizeof(id_type_t));
			     assert($$ != NULL);
			     ((id_type_t *)$$)->size = ((id_type_t *)$1)->size + 1;
			     ((id_type_t *)$$)->type = ((id_type_t *)$1)->type;
			     ((id_type_t *)$$)->dimension = *(int*) $3;
			     ((id_type_t *)$$)->subsize = $1;
			     ((id_type_t *)$1)->supersize = $$;
                            }
     | BASIC                {printf("type-> BASIC\n");
                             $$ = malloc(sizeof(id_type_t));
			     assert($$ != NULL);
			     ((id_type_t *)$$)->type = $1;
			     ((id_type_t *)$$)->size = 0;
			     ((id_type_t *)$$)->dimension = 0;
			     ((id_type_t *)$$)->subsize = NULL;
			     ((id_type_t *)$$)->supersize = NULL;
			    }
     ;

stmts : stmts stmt          {printf("stmts->stmts stmt\n");}
      | /* empty */         {printf("stmts->empty\n");}
      ;

stmt : loc '=' bool ';'                 {printf("stmt->loc = bool\n");}
     | IF '(' bool ')' stmt             {printf("stmt->IF ( bool ) stmt\n");}
     | IF '(' bool ')' stmt ELSE stmt   {printf("stmt->IF ( bool ) stmts ELSE stmt\n");}
     | WHILE '(' bool ')' stmt          {printf("stmt->WHILE ( bool ) stmt\n");}
     | DO stmt WHILE '(' bool ')' ';'   {printf("stmt->DO stmt WHILE ( bool ) ;\n");}
     | BREAK ';'                        {printf("stmt->BREAK ;\n");}
     | block                            {printf("stmt->block\n");}
     ;

loc : loc '[' bool ']'     {printf("loc-> loc [ bool ]\n");}
    | ID                   {printf("loc->ID\n");}
    ;

bool : bool OR join      {printf("bool->bool || join\n");}
     | join              {printf("bool->join\n");}
     ;

join : join AND equality  {printf("join->join && equality\n");}
     | equality           {printf("join->equality\n");}
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


/*
    Function to install a token and corresponding type info
    into the symbol table and return a pointer thereto.
    This function makes a copy of the string it is passed,
    but does not GC the original string. Beware.
*/
void *install_id (char *token, id_type_t *type_info) {
  if (!hash_table_search(sym_table, token)) {
    char *tempstr = malloc (sizeof(char) + strlen(token));
    strcpy (tempstr, token);
    return hash_table_insert(sym_table, (void *)tempstr, type_info);
  }
  return NULL;
}


void print_str (void *str) {
  printf ("%s", (char *) str);
}

void print_id_type (void *id) {
  /*TODO: print out our info on the type here.*/
}


void yyerror (char const * s) {
  fprintf (stderr, "%s\n", s);
  hash_pretty_print (sym_table, print_str, print_id_type);
}


int main () {
  yyin = stdin;

  sym_table = hash_table_init(cmp_string,string_hasher);

  yyparse();

    
}
