%error-verbose

%{
#include "s2struct.h"
#include <stdio.h>
#include <stdlib.h>
%}

%union {
  StringList* strlist;
  char* str;
  float f;
}

%token <str> IDENT
%token <f> NUMBER
%token ELEMENT DENSITY FIXED REACT COLOR COMMA LPAREN RPAREN

%type <strlist> element_list;

%%

spec: 		element_list {destroyStringList($1);}
	|	element_list element_defs

element_list:	IDENT {$$ = cons($1, NULL);}
	|	IDENT COMMA element_list {$$ = cons($1, $3);}

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
int yyerror(const char* str) {
  printf("Error: %s\n", str);
  exit(1);
  return 0;
}

int main(int argc, char** argv) {
  extern FILE* yyin;
  if (argc < 2) return 0;
  yyin = fopen(argv[1], "r");
  
  yyparse();
  
  return 0;
}
