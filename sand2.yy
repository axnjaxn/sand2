%language "C++"
%defines
%locations
%error-verbose

%define parser_class_name "s2parse"

%{
#include <cstdio>
#include <string>

#include "sand2-ctx.h"
%}

%parse-param { sand2_ctx &ctx }
%lex-param   { sand2_ctx &ctx }

%code requires {
    #include <list>
}

%union {
  std::list<std::string>* strlist;
  char* str;
  float f;
}

%token <str> IDENT
%token <f> NUMBER
%token ELEMENT DENSITY FIXED REACT COLOR COMMA LPAREN RPAREN

%type <strlist> element_list;

%{
  extern int yylex(yy::s2parse::semantic_type *yylval, yy::s2parse::location_type* yylloc, sand2_ctx &ctx);
%}
		
%initial-action {
 @$.initialize(&ctx.filename);
 printf("Loading from %s\n", @$.begin.filename->c_str());
}

%%

spec: 	element_list element_defs {ctx.addElements(*$1); delete $1; ctx.print();}

element_list:	IDENT {$$ = new std::list<std::string>(); $$->push_front($1);}
	|	IDENT COMMA element_list {$$ = $3; $$->push_front($1);}

element_defs:	element_def
	|	element_def element_defs

element_def:	ELEMENT IDENT properties

properties:	property
	|	property properties

property:	DENSITY NUMBER
	|	FIXED
	|	COLOR NUMBER COMMA NUMBER COMMA NUMBER
	|	REACT ptable
		
ptable: 	prob_entry
	|	prob_entry COMMA ptable

prob_entry:	IDENT NUMBER

%%
void yyerror(const char* str) {
  printf("Error: %s\n", str);
  exit(1);
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("Usage: %s [definitions file]\n", argv[0]);
    return 0;
  }

  sand2_ctx ctx(argv[1]);
  yy::s2parse parser(ctx);

  parser.parse();

  return 0;
}

namespace yy {
  void s2parse::error(location const &loc, const std::string& s) {
    std::cerr << "error at " << loc << ": " << s << std::endl;
  }
}

