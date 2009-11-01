/* 
   Author: John Gunderman
*/

%{
  #include <stdio.h>
  #include <assert.h>
  
  #define YYSTYPE void *
  
  int yylex (void);

  void yyerror (char const *);

  int int_var = 1;
  int double_var = 2;
  int float_var = 3;

  int true_var = 4;
  int false_var = 5;

    
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
			     ((id_type_t *)$$)->size = *(int*) $3;
			     ((id_type_t *)$$)->type = ((id_type_t *)$1)->type;
			     ((id_type_t *)$$)->dimension = ((id_type_t *)$1)->dimension + 1; 
			     ((id_type_t *)$$)->subsize = $1;
			     ((id_type_t *)$1)->supersize = $$;
                            }
     | BASIC                {printf("type-> BASIC \n");
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

stmt : loc '=' bool ';'                 {printf("stmt->loc = bool\n");
       	       	    			 intmdt_addr_t *dest = malloc (sizeof(intmdt_code_t));
					 if (dest == NULL) {
					    fprintf(stderr,"Error: could not malloc dest in 'loc = bool'");
					    exit(1);
					 }
					 dest->type = symbol;
					 dest->entry_ptr = $1
					 gen(code, "=", $3, NULL, dest);}
     | IF '(' bool ')' stmt             {printf("stmt->IF ( bool ) stmt\n");}
     | IF '(' bool ')' stmt ELSE stmt   {printf("stmt->IF ( bool ) stmts ELSE stmt\n");}
     | WHILE '(' bool ')' stmt          {printf("stmt->WHILE ( bool ) stmt\n");}
     | DO stmt WHILE '(' bool ')' ';'   {printf("stmt->DO stmt WHILE ( bool ) ;\n");}
     | BREAK ';'                        {printf("stmt->BREAK ;\n");}
     | block                            {printf("stmt->block\n");}
     ;

loc : loc '[' bool ']'     {printf("loc-> loc [ bool ]\n");
      	      	   	    $$ = ((list_entry_t*)$1)->subsize;
			    /* TODO: figure out what the value of the bool is and pass it */ }
    | ID                   {printf("loc->ID\n");
                            list_entry_t *id = hash_table_search(sym_table, $1);
			    if (id == NULL) {
			      fprintf(stderr, "Error: symbol '%s' was not previously defined.\n", (char*)$1);
			      exit(1);
			    }
                            $$ = id;}
    ;

bool : bool OR join      {printf("bool->bool || join\n");
       	     	          intmdt_addr_t *temp = newtemp(env);
			  gen(code, "||", $1, $3, temp);
			  $$ = temp;}
     | join              {printf("bool->join\n");
                          $$ = $1;}
     ;

join : join AND equality  {printf("join->join && equality\n");
       	     	           intmdt_addr_t *temp = newtemp(env);
			   gen(code, "&&", $1, $3, temp);
			   $$ = temp;}
     | equality           {printf("join->equality\n");
                           $$ = $1;}
     ;

equality : equality EQ rel      {printf("equality->equality == rel\n");
       	     		         intmdt_addr_t *temp = newtemp(env);
			    	 gen(code, "==", $1, $3, temp);
			    	 $$ = temp;}
	 | equality NE rel      {printf("equality->equality != rel\n");
       	     		         intmdt_addr_t *temp = newtemp(env);
			    	 gen(code, "!=", $1, $3, temp);
			    	 $$ = temp;}
	 | rel                  {printf("equality->rel\n");
	                         $$ = $1}
	 ;

rel : expr LT expr         {printf("rel->expr < expr\n");
       	     		    intmdt_addr_t *temp = newtemp(env);
			    gen(code, "<=", $1, $3, temp);
			    $$ = temp;}
    | expr LE expr         {printf("rel->expr <= expr\n");
       	     		    intmdt_addr_t *temp = newtemp(env);
			    gen(code, "<", $1, $3, temp);
			    $$ = temp;}
    | expr GE expr         {printf("rel->expr >= expr\n");
       	     		    intmdt_addr_t *temp = newtemp(env);
			    gen(code, ">=", $1, $3, temp);
			    $$ = temp;}
    | expr GT expr         {printf("rel->expr > expr\n");
       	     		    intmdt_addr_t *temp = newtemp(env);
			    gen(code, ">", $1, $3, temp);
			    $$ = temp;}
    | expr                 {printf("rel->expr\n");
                            $$ = $1}
    ;

expr : expr '+' term       {printf("expr->expr + expr\n");
       	     		    intmdt_addr_t *temp = newtemp(env);
			    gen(code, "+", $1, $3, temp);
			    $$ = temp;}
     | expr '-' term       {printf("expr->expr - expr\n");
       	     		    intmdt_addr_t *temp = newtemp(env);
			    gen(code, "-", $1, $3, temp);
			    $$ = temp;}
     | term                {printf("expr->term\n");
                            $$ = $1}
     ;

term : term '*' urnary     {printf("term->term * urnary\n");
       	     		    intmdt_addr_t *temp = newtemp(env);
			    gen(code, "*", $1, $3, temp);
			    $$ = temp;}
     | term '/' urnary     {printf("term->term / urnary\n");
       	     		    intmdt_addr_t *temp = newtemp(env);
			    gen(code, "/", $1, $3, temp);
			    $$ = temp;}
     | urnary              {printf("term->urnary\n");
       			    $$ = $1;}
     ;

urnary : '!' urnary        {printf("urnary-> ! urnary\n");
       	     		    intmdt_addr_t *temp = newtemp(env);
			    gen(code, "!", $2, NULL, temp);
			    $$ = temp;
			    /* TODO: I think this may need type checking */}
       | '-' urnary        {printf("urnary-> - urnary\n");
                            intmdt_addr_t *temp = newtemp(env);
			    gen(code, "-", $2, NULL, temp);
			    $$ = temp;}
       | factor            {printf("urnary->factor\n");
                            $$ = $1;}
       ;
				   
factor : '(' bool ')'      {printf("factor->( bool )\n");
	                    $$ = $2;}
       | loc               {printf("factor->loc\n");
                            intmdt_addr_t *dest = malloc (sizeof(intmdt_code_t));
			    if (dest == NULL) {
			      fprintf(stderr,"Error: could not malloc dest in 'loc = bool'");
			      exit(1);
			    }
			    dest->type = symbol;
			    dest->entry_ptr = $1;
	                    $$ = dest;}
       | NUM               {printf("factor->NUM\n");
                            intmdt_addr_t *temp = newtemp(env);
	                    temp->entry_ptr->value->type = &int_var;
	                    $$ = temp;}
       | REAL              {printf("factor->REAL\n");
                            intmdt_addr_t *temp = newtemp(env);
	                    temp->entry_ptr->value->type = &float_var;
	                    $$ = temp;}
       | TRUE              {printf("factor->TRUE\n");
                            intmdt_addr_t *temp = newtemp(env);
	                    temp->entry_ptr->value->type = &true_var;
	                    $$ = temp;}
       | FALSE             {printf("factor->FALSE\n");
                            intmdt_addr_t *temp = newtemp(env);
	                    temp->entry_ptr->value->type = &false_var;
	                    $$ = temp;}
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


/* 
   Print out a string. This function is meant to be passed to
   hash_pretty_print().
*/
void print_str (void *str) {
  printf ("%s", (char *) str);
}

/*
  Print out the contents of the id_type
  of the symbol in the symbol table.
  This function is meant to be passed
  to hash_pretty_print();
*/
void print_id_type (void *id) {
  printf("{");
  switch (*(unsigned int *)((id_type_t *)id)->type) {
  case 1:
    printf("&int_var");
    break;
  case 2:
    printf("&double_var");
    break;
  case 3:
    printf("&float_var");
    break;
  case 4:
    printf("&true_var");
    break;
  case 5:
    printf("&false_var");
    break;
  default:
    printf("unknown_type");
  } 
  printf(",%d,%d,",
	 ((id_type_t *)id)->dimension,
	 ((id_type_t *)id)->size);
  if (((id_type_t *)id)->subsize == NULL) {
    printf ("NULL");
  } else {
    print_id_type (((id_type_t *)id)->subsize);
  }
  /* print out subsize here (as string) */
  /*TODO: print out our info on the type here.*/
  if (((id_type_t *)id)->supersize == NULL) {
    printf (",NULL}");
  } else {
    printf(",%p}", ((id_type_t *)id)->supersize);
  }
}


/* 
   This function is automagically called by yacc/bison
   when a syntax error is encountered. It also can be
   called manually to specially handle syntax errors.
*/
void yyerror (char const * s) {
  fprintf (stderr, "%s\n", s);
  hash_pretty_print (sym_table, print_str, print_id_type);
}


int main () {
  yyin = fopen("test.code", "r");

  sym_table = hash_table_init(cmp_string,string_hasher);

  yyparse();
  
  printf("\n\n");
  hash_pretty_print (sym_table, print_str, print_id_type);

    
}
