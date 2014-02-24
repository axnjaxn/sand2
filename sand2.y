%error-verbose

%{
#include "s2struct.h"
#include <stdio.h>
#include <stdlib.h>

Sand2Spec* gspec = NULL;
%}

%union {
  struct _PropertyTable* s2prop;
  struct _S2List* s2list;
  struct _Sand2Spec* s2spec;
  char* str;
  float f;
  int i;
}

%token <str> IDENT
%token <f> NUMBER
%token MENU ELEMENT DENSITY PRESSURE REACT DECAY COLOR COMMA LPAREN RPAREN COLON

%type <s2spec> spec
%type <s2list> menu_defs element_defs
%type <s2prop> properties
%type <s2list> ptable

%%

spec: 		MENU menu_defs element_defs {$$ = gspec = makeSpec($3, $2);}
	|	element_defs {$$ = gspec = makeSpec($1, NULL);}

menu_defs:	IDENT {$$ = cons($1, NULL);}
	|	IDENT COMMA menu_defs {$$ = cons($1, $3);}

element_defs:	{$$ = NULL;}
	|	ELEMENT IDENT properties element_defs {$$ = addProperties($2, $3, $4);}

properties:	{$$ = defaultPropertyTable();}
	|	COLOR NUMBER COMMA NUMBER COMMA NUMBER properties {$$ = $7; $$->argb = toARGB($2, $4, $6);}
	|	DENSITY NUMBER properties {$$ = $3; $$->density = $2; $$->fixed = 0;}
	|	PRESSURE NUMBER properties {$$ = $3; $$->pressure = (int)($2);}
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
