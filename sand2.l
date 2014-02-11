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
"element" return ELEMENT;
"density" return DENSITY;
"fixed" return FIXED;
"color" return COLOR;
"react" return REACT;
"(" return LPAREN;
")" return RPAREN;
"," return COMMA;
[a-zA-Z_][0-9a-zA-Z_]* {yylval.str = strdup(yytext); return IDENT;}
[0-9\-\+][0-9]*[0-9\.][0-9]* {sscanf(yytext, "%f", &yylval.f); return NUMBER;}

  /* Catch-all */
. {char err[256]; sprintf(err, "Unrecognized symbol: %s\n", yytext); yyerror(err);}
%%
