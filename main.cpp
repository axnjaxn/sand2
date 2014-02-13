extern "C" {
#include "sand2.tab.h"
#include "s2struct.h"
  int yyparse();
}

#include <cstdio>
#include "world.h"

int main(int argc, char* argv[]) {
  extern FILE* yyin;
  if (argc < 2) return 0;
  yyin = fopen(argv[1], "r");

  extern Sand2Spec* gspec;
  yyparse();

  ElementTable table(gspec);
  destroySpec(gspec);

  World world(&table, 640, 480);
  
  return 0;
}
