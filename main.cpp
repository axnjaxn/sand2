extern "C" {
#include "sand2.tab.h"
#include "s2struct.h"
  int yyparse();
}

#include <cstdio>
#include "world.h"
#include "pixelrenderer.h"

#define TITLE "Sand2 by Brian Jackson"

int main(int argc, char* argv[]) {
  const int w = 640, h = 480;

  extern FILE* yyin;
  if (argc < 2) return 0;
  yyin = fopen(argv[1], "r");

  extern Sand2Spec* gspec;
  yyparse();

  ElementTable table(gspec);
  destroySpec(gspec);

  World world(&table, w, h);

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

  SDL_Event event;
  bool exitflag = 0;
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

    SDL_Delay(5);    
  }

  return 0;
}

