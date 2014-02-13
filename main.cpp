extern "C" {
#include "sand2.tab.h"
#include "s2struct.h"
  int yyparse();
}

#include <cstdlib>
#include <cstdio>
#include <ctime>
#include "world.h"
#include "pixelrenderer.h"

#define TITLE "Sand2 by Brian Jackson"

int main(int argc, char* argv[]) {
  const int w = 640, h = 480;

  srand(time(NULL));

  extern FILE* yyin;
  if (argc < 2) return 0;
  yyin = fopen(argv[1], "r");

  extern Sand2Spec* gspec;
  yyparse();

  ElementTable table(gspec);
  destroySpec(gspec);

  World world(&table, h, w);

  //Initialize SDL and catch init errors
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("Fatal error initializing SDL");
    return 1;
  }
  atexit(SDL_Quit);
  
  SDL_Window* window = SDL_CreateWindow(TITLE,
					SDL_WINDOWPOS_CENTERED,
					SDL_WINDOWPOS_CENTERED,
					w, h,
					0);
  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

  PixelRenderer* px = new PixelRenderer(renderer, w, h);
  SDL_RenderSetLogicalSize(renderer, w, h);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderClear(renderer);
  px->redraw();

  SDL_RenderPresent(renderer);

  int radius = 5, mx, my;
  ElementID mode = 2;

  SDL_Event event;
  bool exitflag = 0, mousedown = 0;
  while (!exitflag) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) exitflag = 1;
      else if (event.type == SDL_KEYDOWN) {
	switch (event.key.keysym.sym) {
	default: break;
	case SDLK_ESCAPE: exitflag = 1; break;
	}
      }
    }

    if (SDL_GetMouseState(&mx, &my)&SDL_BUTTON(1)) {
      for (int bw, by = -radius; by <= radius; by++) {
	bw = (int)(sqrt(radius * radius - by * by) - 0.75);
	for (int bx = -bw; bx <= bw; bx++)
	  world.set(by + my, bx + mx, mode);
      }
      world.flipBuffer();
    }
    
    world.iterate();

    for (int r = 0; r < world.nr; r++)
      for (int c = 0; c < world.nc; c++)
	px->set(r, c, table.elements[world.at(r, c)].argb);
    px->redraw();

    SDL_RenderPresent(renderer);
    
    SDL_Delay(5);    
  }

  delete px;

  return 0;
}

