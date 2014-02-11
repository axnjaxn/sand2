%error-verbose

%{
#include "s2struct.h"
#include <stdio.h>
#include <stdlib.h>

Sand2Spec* gspec = NULL;
%}

%union {
  PropertyTable* s2prop;
  S2List* s2list;
  Sand2Spec* s2spec;
  char* str;
  float f;
  int i;
}

%token <str> IDENT
%token <f> NUMBER
%token ELEMENT DENSITY REACT DECAY COLOR COMMA LPAREN RPAREN COLON

%type <s2spec> spec;
%type <s2list> element_list element_defs
%type <s2prop> properties
%type <s2list> ptable

%%

spec: 		element_list element_defs {$$ = gspec = makeSpec($1, $2);}

element_list:	IDENT {$$ = cons($1, NULL);}
	|	IDENT COMMA element_list {$$ = cons($1, $3);}

element_defs:	{$$ = NULL;}
	|	ELEMENT IDENT properties element_defs {$$ = addProperties($2, $3, $4);}

properties:	{$$ = defaultPropertyTable();}
	|	COLOR NUMBER COMMA NUMBER COMMA NUMBER properties {$$ = $7; $$->argb = toARGB($2, $4, $6);}
	|	DENSITY NUMBER properties {$$ = $3; $$->density = $2; $$->fixed = 0;}
	|	REACT IDENT COLON ptable properties {$$ = $5; $$->reactions = addList($2, $4, $$->reactions);}
	| 	DECAY ptable properties {$$ = $3; if ($$->decay) yyerror("Multiple decays declared for element."); $$->decay = $2;}

ptable: 	IDENT NUMBER {$$ = addProb($1, $2, NULL);}
	|	IDENT NUMBER COMMA ptable {$$ = addProb($1, $2, $4);}

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

  extern Sand2Spec* gspec;
  yyparse();
  destroySpec(gspec);
  
  return 0;
}
