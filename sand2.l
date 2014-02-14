%option nodefault noyywrap case-insensitive yylineno
%{
# include <stdlib.h>

# include "s2struct.h"
# include "sand2.tab.h"

int yyerror(const char*);
 
%}

%%
"//".* /* Comments ignored */
[" "\t\r\n] /* Whitespace ignored */
<<EOF>> {yyterminate();}

  /* Basic tokens */
"menu" return MENU;
"element" return ELEMENT;
"density" return DENSITY;
"color" return COLOR;
"react" return REACT;
"decay" return DECAY;
"(" return LPAREN;
")" return RPAREN;
"," return COMMA;
":" return COLON;
[a-zA-Z_][0-9a-zA-Z_]* {yylval.str = strdup(yytext); return IDENT;}
[0-9\-\+][0-9]*[0-9\.][0-9]* {sscanf(yytext, "%f", &yylval.f); return NUMBER;}

  /* Catch-all */
. {char err[256]; sprintf(err, "Unrecognized symbol: %s\n", yytext); yyerror(err);}
%%
