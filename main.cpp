extern "C" {
#include "sand2.tab.h"
#include "s2struct.h"
  int yyparse();
}

#include <cstdio>
#include "elements.h"

int main(int argc, char* argv[]) {
  extern FILE* yyin;
  if (argc < 2) return 0;
  yyin = fopen(argv[1], "r");

  extern Sand2Spec* gspec;
  yyparse();


  ElementTable table(gspec);
  destroySpec(gspec);

  printf("Elements:\n");
  for (int i = 0; i < table.elements.size(); i++)
    printf("%s\n", table.elements[i].name.c_str());
  
  return 0;
}
