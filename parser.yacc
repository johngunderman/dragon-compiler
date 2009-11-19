/* 
   Author: John Gunderman
*/

%{
  #include <stdio.h>
  #include <assert.h>
  
  #include "intmdt_code_gen.h"

  #define YYSTYPE void *
  
  int yylex (void);

  void yyerror (char const *);

    
  void *install_id (char *, struct id_type_t *);

  intmdt_code_t *intermediate_code;
  env_t *env;

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
       	       		     /*TODO: check if id has already been declared */
                             install_id((char*)$2, (id_type_t*)$1); 
			    }
     ; 	     

type : type '[' NUM ']'     {printf("type-> type [ NUM ]\n");
                             $$ =  malloc(sizeof(id_type_t));
			     assert($$ != NULL);
			     ((id_type_t *)$$)->size = *(int*) $3;
			     ((id_type_t *)$$)->type = ((id_type_t *)$1)->type;
			     ((id_type_t *)$$)->dimension = ((id_type_t *)$1)->dimension + 1; 
			     ((id_type_t *)$1)->supersize = $$;
			     ((id_type_t *)$$)->subsize = $1;
			     
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
					 dest->addr.entry_ptr = $1;
					 gen(intermediate_code, "=", $3, NULL, dest);}
     | IF '(' bool ')' m stmt             {printf("stmt->IF ( bool ) stmt\n");
       	      	       		 	   backpatch(((boolean_list_t*) $3)->truelist, intermediate_code->code[(int) $5]);
       					   }//$$ = list_merge(((boolean_list_t*) $3)->falselist, $6);}
     | IF '(' bool ')' m stmt n ELSE m stmt   {printf("stmt->IF ( bool ) stmts ELSE stmt\n");
       	      	       		      	       backpatch(((boolean_list_t*) $3)->truelist, intermediate_code->code[(int) $5]);
					       backpatch(((boolean_list_t*) $3)->falselist, intermediate_code->code[(int) $9]);
					       //boolean_list_t *temp = list_merge($6, $7);
					       }//$$ =  list_merge(temp, $10);}
     | WHILE m '(' bool ')' m stmt          {printf("stmt->WHILE ( bool ) stmt\n");
       	     	      	  		   //backpatch($7, intermediate_code->code[(int) $2]);
					   backpatch(((boolean_list_t*)$4)->truelist, intermediate_code->code[(int) $6]);
					   $$ = ((boolean_list_t*)$4)->falselist;
					   gen(intermediate_code, "goto", NULL, NULL, intermediate_code->code[(int) $2]); }
     | DO stmt WHILE '(' bool ')' ';'   {printf("stmt->DO stmt WHILE ( bool ) ;\n");}
     | BREAK ';'                        {printf("stmt->BREAK ;\n");
       	     				 gen(intermediate_code, "goto", NULL, NULL, NULL);}
     | startscope block                 {printf("stmt->block\n");
                                         printf("\n\nLeaving Scope\n\n");
                                         env = pop_env_table(env);}
     ;

startscope : /* EMPTY */ {printf("\n\nEntering New Scope\n\n");
                          env = push_env_table(env);}
           ;

n : /* EMPTY */ {printf("I should be doing stuff right now...");
       	     	 gen(intermediate_code, "goto", NULL, NULL, NULL);
		 $$ = list_makelist(intermediate_code->code[intermediate_code->n - 1]);}
  ;


loc : loc '[' bool ']'     {printf("loc-> loc [ bool ]\n");
                            /* Looks gross, but all it does is assign the subsize of the current id_type as the value of the list_entry  */
                            ((list_entry_t*)$1)->value = ((id_type_t*)((list_entry_t*)$1)->value)->subsize;
			     $$ = $1;
			    /* TODO: figure out what the value of the bool is and pass it */ }
    | ID                   {printf("loc->ID\n");
                            id_type_t *id = env_search(env, $1);
			    if (id == NULL) {
			      fprintf(stderr, "Error: symbol '%s' was not previously defined.\n", (char*)$1);
			      exit(1);
			    }
			    list_entry_t *entry = malloc (sizeof(list_entry_t));
			    entry->key = $1;
			    entry->value = id;
                            $$ = entry;}
    ;

bool : bool OR m join      {printf("bool->bool || join\n");
       	     	                    boolean_list_t *list = malloc(sizeof(boolean_list_t));
   			    	     if (list == NULL) {
			      	       fprintf(stderr, "malloc of boolean_list_t failed!\n");
			      	       exit(1);
			    	     }
				     backpatch( ((boolean_list_t*) $1)->truelist, intermediate_code->code[(int) $3]);
				     intmdt_addr_t *temp = newtemp(env);
			   	     gen(intermediate_code, "||", $1, $3, temp);
			   	     $$ = temp;}
     | join                        {printf("bool->join\n");
                                    $$ = $1;}
     ;

join : join AND m equality  {printf("join->join && equality\n");
       	     	          	     boolean_list_t *list = malloc(sizeof(boolean_list_t));
   			    	     if (list == NULL) {
			      	       fprintf(stderr, "malloc of boolean_list_t failed!\n");
			      	       exit(1);
			    	     }
				     backpatch( ((boolean_list_t*) $1)->truelist, intermediate_code->code[(int) $3]);
				     intmdt_addr_t *temp = newtemp(env);
			   	     gen(intermediate_code, "&&", $1, $3, temp);
			   	     $$ = temp;}
     | equality           	    {printf("join->equality\n");
                           	     $$ = $1;}
     ;

equality : equality EQ rel      {printf("equality->equality == rel\n");
                                 boolean_list_t *list = malloc(sizeof(boolean_list_t));
   			    	 if (list == NULL) {
			      	   fprintf(stderr, "malloc of boolean_list_t failed!\n");
			      	   exit(1);
			    	 }
                            	 widen(intermediate_code, env, $1, ((intmdt_addr_t*) $3)->addr.entry_ptr->value);
       	     		   	 intmdt_addr_t *temp = newtemp(env);
			    	 gen(intermediate_code, "==", $1, $3, temp);
			    	 list->truelist = list_makelist(intermediate_code->code[intermediate_code->n - 1]);
			    	 gen(intermediate_code, "goto", NULL, NULL, NULL);
			    	 list->falselist = list_makelist(intermediate_code->code[intermediate_code->n - 1]);
			    	 $$ = list;}
	 | equality NE rel      {printf("equality->equality != rel\n");
                                 boolean_list_t *list = malloc(sizeof(boolean_list_t));
   			    	 if (list == NULL) {
			      	   fprintf(stderr, "malloc of boolean_list_t failed!\n");
			      	   exit(1);
			    	 }
                            	 widen(intermediate_code, env, $1, ((intmdt_addr_t*) $3)->addr.entry_ptr->value);
       	     		   	 intmdt_addr_t *temp = newtemp(env);
			    	 gen(intermediate_code, "!=", $1, $3, temp);
			    	 list->truelist = list_makelist(intermediate_code->code[intermediate_code->n - 1]);
			    	 gen(intermediate_code, "goto", NULL, NULL, NULL);
			    	 list->falselist = list_makelist(intermediate_code->code[intermediate_code->n - 1]);
			    	 $$ = list;}
	 | rel                  {printf("equality->rel\n");
                                 $$ = $1;}
	 ;

rel : expr LT expr         {printf("rel->expr < expr\n");
      	      		    boolean_list_t *list = malloc(sizeof(boolean_list_t));
   			    if (list == NULL) {
			      fprintf(stderr, "malloc of boolean_list_t failed!\n");
			      exit(1);
			    }
                            widen(intermediate_code, env, $1, ((intmdt_addr_t*) $3)->addr.entry_ptr->value);
       	     		    intmdt_addr_t *temp = newtemp(env);
			    gen(intermediate_code, "<", $1, $3, temp);
			    list->truelist = list_makelist(intermediate_code->code[intermediate_code->n - 1]);
			    gen(intermediate_code, "goto", NULL, NULL, NULL);
			    list->falselist = list_makelist(intermediate_code->code[intermediate_code->n - 1]);
			    $$ = list;}
    | expr LE expr         {printf("rel->expr <= expr\n");
                            boolean_list_t *list = malloc(sizeof(boolean_list_t));
   			    if (list == NULL) {
			      fprintf(stderr, "malloc of boolean_list_t failed!\n");
			      exit(1);
			    }
                            widen(intermediate_code, env, $1, ((intmdt_addr_t*) $3)->addr.entry_ptr->value);
       	     		    intmdt_addr_t *temp = newtemp(env);
			    gen(intermediate_code, "<=", $1, $3, temp);
			    list->truelist = list_makelist(intermediate_code->code[intermediate_code->n - 1]);
			    gen(intermediate_code, "goto", NULL, NULL, NULL);
			    list->falselist = list_makelist(intermediate_code->code[intermediate_code->n - 1]);
			    $$ = list;}
    | expr GE expr         {printf("rel->expr >= expr\n");
                            boolean_list_t *list = malloc(sizeof(boolean_list_t));
   			    if (list == NULL) {
			      fprintf(stderr, "malloc of boolean_list_t failed!\n");
			      exit(1);
			    }
                            widen(intermediate_code, env, $1, ((intmdt_addr_t*) $3)->addr.entry_ptr->value);
       	     		    intmdt_addr_t *temp = newtemp(env);
			    gen(intermediate_code, ">=", $1, $3, temp);
			    list->truelist = list_makelist(intermediate_code->code[intermediate_code->n - 1]);
			    gen(intermediate_code, "goto", NULL, NULL, NULL);
			    list->falselist = list_makelist(intermediate_code->code[intermediate_code->n - 1]);
			    $$ = list;}
    | expr GT expr         {printf("rel->expr > expr\n");
                            boolean_list_t *list = malloc(sizeof(boolean_list_t));
   			    if (list == NULL) {
			      fprintf(stderr, "malloc of boolean_list_t failed!\n");
			      exit(1);
			    }
                            widen(intermediate_code, env, $1, ((intmdt_addr_t*) $3)->addr.entry_ptr->value);
       	     		    intmdt_addr_t *temp = newtemp(env);
			    gen(intermediate_code, ">", $1, $3, temp);
			    list->truelist = list_makelist(intermediate_code->code[intermediate_code->n - 1]);
			    gen(intermediate_code, "goto", NULL, NULL, NULL);
			    list->falselist = list_makelist(intermediate_code->code[intermediate_code->n - 1]);
			    $$ = list;}
    | expr                 {printf("rel->expr\n");
                            $$ = $1;}
    ;

expr : expr '+' term       {printf("expr->expr + expr\n");
                            widen(intermediate_code, env, $1, ((intmdt_addr_t*) $3)->addr.entry_ptr->value);
       	     		    intmdt_addr_t *temp = newtemp(env);
			    gen(intermediate_code, "+", $1, $3, temp);
			    $$ = temp;}
     | expr '-' term       {printf("expr->expr - expr\n");
                            widen(intermediate_code, env, $1, ((intmdt_addr_t*) $3)->addr.entry_ptr->value);
       	     		    intmdt_addr_t *temp = newtemp(env);
			    gen(intermediate_code, "-", $1, $3, temp);
			    $$ = temp;}
     | term                {printf("expr->term\n");
                            $$ = $1;}
     ;

term : term '*' urnary     {printf("term->term * urnary\n");
                            widen(intermediate_code, env, $1, ((intmdt_addr_t*) $3)->addr.entry_ptr->value);
       	     		    intmdt_addr_t *temp = newtemp(env);
			    gen(intermediate_code, "*", $1, $3, temp);
			    $$ = temp;}
     | term '/' urnary     {printf("term->term / urnary\n");
                            widen(intermediate_code, env, $1, ((intmdt_addr_t*) $3)->addr.entry_ptr->value);
       	     		    intmdt_addr_t *temp = newtemp(env);
			    gen(intermediate_code, "/", $1, $3, temp);
			    $$ = temp;}
     | urnary              {printf("term->urnary\n");
       			    $$ = $1;}
     ;

urnary : '!' urnary        {printf("urnary-> ! urnary\n");
       	     		    boolean_list_t *list = malloc(sizeof(boolean_list_t));
   			    if (list == NULL) {
			      fprintf(stderr, "malloc of boolean_list_t failed!\n");
			      exit(1);
			    }
			    list->truelist = ((boolean_list_t*)$2)->falselist;
			    list->falselist = ((boolean_list_t*)$2)->truelist;
			    $$ = list;}
			    /* TODO: I think this may need type checking */
       | '-' urnary        {printf("urnary-> - urnary\n");
                            intmdt_addr_t *temp = newtemp(env);
			    gen(intermediate_code, "-", $2, NULL, temp);
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
			    dest->addr.entry_ptr = $1;
	                    $$ = dest;}
       | NUM               {printf("factor->NUM\n");
                            intmdt_addr_t *temp = newtemp(env);
	                    ((id_type_t*)temp->addr.entry_ptr->value)->type = &int_var;
	                    $$ = temp;}
       | REAL              {printf("factor->REAL\n");
                            intmdt_addr_t *temp = newtemp(env);
	                    ((id_type_t*)temp->addr.entry_ptr->value)->type = &float_var;
	                    $$ = temp;}
       | TRUE              {printf("factor->TRUE\n");
                            boolean_list_t *list = malloc(sizeof(boolean_list_t));
   			    if (list == NULL) {
			      fprintf(stderr, "malloc of boolean_list_t failed!\n");
			      exit(1);
			    }
			    /* Generate a goto statement with no destination,
			       then pass it into a list for backpatching. */
			    gen(intermediate_code, "goto", NULL, NULL, NULL);
			    list->truelist = list_makelist(intermediate_code->code[intermediate_code->n - 1]);
			    list->falselist = NULL;
	                    $$ = list;}
       | FALSE             {printf("factor->FALSE\n");
			    boolean_list_t *list = malloc(sizeof(boolean_list_t));
   			    if (list == NULL) {
			      fprintf(stderr, "malloc of boolean_list_t failed!\n");
			      exit(1);
			    }
			    /* Generate a goto statement with no destination,
			       then pass it into a list for backpatching. */
			    gen(intermediate_code, "goto", NULL, NULL, NULL);
			    list->falselist = list_makelist(intermediate_code->code[intermediate_code->n - 1]);
			    list->truelist = NULL;
	                    $$ = list;}
       ;

m : /* empty */    {printf("getting next instruction...\n");
                            $$ = (void *) intermediate_code->n;}



%%

#include "lex.yy.c"


/*
    Function to install a token and corresponding type info
    into the symbol table and return a pointer thereto.
    This function makes a copy of the string it is passed,
    but does not GC the original string. Beware.
*/
void *install_id (char *token, id_type_t *type_info) {
  if (!hash_table_search(env->table, token)) {
    char *tempstr = malloc (sizeof(char) + strlen(token));
    strcpy (tempstr, token);
    return hash_table_insert(env->table, (void *)tempstr, type_info);
  }
  return NULL;
}


/* 
   This function is automagically called by yacc/bison
   when a syntax error is encountered. It also can be
   called manually to specially handle syntax errors.
*/
void yyerror (char const * s) {
  fprintf (stderr, "%s\n", s);
}


int main () {
  yyin = fopen("test.code", "r");

  intermediate_code = init_code();
  env = init_env();
  


  yyparse();
  
  printf("\n\n");

  print_env(env);

  intmdt_code_print(intermediate_code);

}

