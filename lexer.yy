%{
    /*
    LT, LE, EQ, NE, QT, GE,
    IF, THEN, ELSE, ID, NUMBER, RELOP
    */
%}

/* Regular Definitions */
delim   [ \t\n]
ws      {delim}+
letter  [A-Za-z]
digit   [0-9]
id      {letter}({letter}|{digit})*
number {digit}+(\.{digit}+)?(E[+-]?{digit}+)?

%%

{ws}    {/* no action and no return */}
if      {return(IF);}
then    {return(THEN);}
else    {return(ELSE):}
{id}    {yyval = (int) install_id(); return(ID);}
{number}{yyval = (int) install_num(); return (NUMBER);}
"<"     {yyval = LT; return (RELOP);}
"<="    {yyval = LE; return (RELOP);}
"="     {yyval = EQ; return (RELOP);}
"<>"    {yyval = NE; return (RELOP);}
">"     {yyval = GT; return (RELOP);}
">="    {yyval = GE; return (RELOP);}

%%


/*
    Function to install the lexeme, whose first character
    is pointed to by yytext, and whose length is yyleng,
    into the symbol table and return a pointer thereto.
*/
int install_id();

/*
    Similar to install_id, but puts numerical constants into
    a separate table.
*/
int install_num();
