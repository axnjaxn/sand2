%option nodefault noyywrap case-insensitive yylineno nounput
%{
# include <cstdlib>

# include "sand2-ctx.h"
# include "sand2.tab.hh"

#define YY_DECL int yylex(yy::s2parse::semantic_type *yylval, yy::s2parse::location_type *yylloc, sand2_ctx &ctx)

// make location include the current token
# define YY_USER_ACTION  yylloc->columns (yyleng);

typedef yy::s2parse::token token;

void yyerror(const char*);

%}

%%

%{
     // start where previous token ended
     yylloc->step();
%}

"//".* /* Comments ignored */
[" "\t\r\n] /* Whitespace ignored */
<<EOF>> {printf("EOF Detected\n");}

  /* Basic tokens */
"element" return token::ELEMENT;
"density" return token::DENSITY;
"fixed" return token::FIXED;
"color" return token::COLOR;
"react" return token::REACT;
"(" return token::LPAREN;
")" return token::RPAREN;
"," return token::COMMA;
[a-zA-Z_][0-9a-zA-Z_]* {yylval->str = strdup(yytext); return token::IDENT;}
[0-9\-\+][0-9]*[0-9\.][0-9]* {sscanf(yytext, "%f", &yylval->f); return token::NUMBER;}

  /* Catch-all */
. {char err[256]; sprintf(err, "Unrecognized symbol: %s\n", yytext); yyerror(err);}
%%
