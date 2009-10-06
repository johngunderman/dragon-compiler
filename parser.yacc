/* Reverse polish notation calculator.  */

%{
  #define YYSTYPE double

  int yylex (void);
  void yyerror (char const *);
  %}

%token NUM

%% /* Grammar rules and actions follow.  */


program : block
	;

block : '{' decls stmts '}'
      ;

decls : decls decl
      | /* empty */
      ;

decl : type ID ';'
     ;

type : type '[' NUM ']'
     | BASIC
     ;

stmts : stmts stmt
      | /* empty */
      ;

stmt : loc '=' bool;
     | IF '(' bool ')' stmt
     | IF '(' bool ')' stmt ELSE stmt
     | WHILE '(' bool ')' stmt
     | DO stmt WHILE '(' bool ')' ';'
     | BREAK ';'
     | block
     ;

loc : loc '[' bool ']'
    | ID
    ;

bool : bool '||' join
     | join
     ;

join : join '&&' equality
     | equality
     ;

equality : equality '==' rel
	 | equality '!=' rel
	 | rel
	 ;

rel : expr '<' expr
    | expr '<=' expr
    | expr '>=' expr
    | expr '>' expr
    | expr
    ;

expr : expr '+' term
     | expr '-' term
     | term
     ;

term : term '*' urnary
     | term '/' urnary
     | urnary
     ;

urnary : '!' urnary
       | '-' urnary
       | factor
       ;

factor : '(' bool ')'
       | loc
       | NUM
       | REAL
       | TRUE
       | FALSE
       ;

