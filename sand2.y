%error-verbose

%{
#include "s2struct.h"
#include <stdio.h>
%}

%union {
  struct StringList* strlist;
  char* str;
  float f;
}

%token <str> IDENT
%token <f> NUMBER
%token ELEMENT DENSITY FIXED REACT COLOR COMMA LPAREN RPAREN

%type <strlist> element_list;

%%

spec: 	element_list element_defs

element_list:	IDENT
	|	IDENT COMMA element_list

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
