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
#include "osd.h"

#define TITLE "Sand2 by Brian Jackson"

#include <sstream>
template <typename tn>
std::string toString(const tn& t) {
  std::ostringstream oss;
  oss << t;
  return oss.str();
}

void drawElement(World& world, ElementID mode, int x, int y, int radius) {
  for (int bw, by = -radius; by <= radius; by++) {
    bw = (int)(sqrt(radius * radius - by * by) - 0.75);
    for (int bx = -bw; bx <= bw; bx++)
      world.set(by + y, bx + x, mode);
  }
}

void drawElement(World& world, ElementID mode, int x0, int y0, int x1, int y1, int radius) {
  float dx = x1 - x0;
  float dy = y1 - y0;
  float d = sqrt(dx * dx + dy * dy);
  if (d == 0.0) d = 1;
  dx /= d;
  dy /= d;

  for (int i = 0, x, y; i <= d; i++) {
    x = (int)(x0 + i * dx + 0.5);
    y = (int)(y0 + i * dy + 0.5);
    drawElement(world, mode, x, y, radius);
  }    
}

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

  OSD osd;
  osd.setText("Arrows to change element / size", 5000);
  const int osd_time = 2000;
  
  int radius = 7, mx, my, nx, ny;
  ElementID mode = 2;

  SDL_Event event;
  bool exitflag = 0, mousedown = 0;
  Uint32 ticks = 0, dticks;
  while (!exitflag) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) exitflag = 1;
      else if (event.type == SDL_KEYDOWN) {
	switch (event.key.keysym.sym) {
	default: break;
	case SDLK_ESCAPE: exitflag = 1; break;
	case SDLK_BACKSPACE: world.clear(); osd.setText("World cleared!", osd_time); break;
	case SDLK_f:
	  osd.setText(toString(1000.0 / dticks) + " FPS", osd_time);
	  break;
	case SDLK_UP:
	  radius++; 
	  osd.setText("Radius: " + toString(radius), osd_time); 
	  break;
	case SDLK_DOWN: 
	  radius--; 
	  osd.setText("Radius: " + toString(radius), osd_time); 
	  break;
	case SDLK_LEFT:
	  mode = (mode + table.elements.size() - 1) % table.elements.size();
	  osd.setText("Element: " + table.elements[mode].name, osd_time);
	  break;
	case SDLK_RIGHT: 
	  mode = (mode + 1) % table.elements.size(); 
	  osd.setText("Element: " + table.elements[mode].name, osd_time);
	  break;
	}
      }
    }

    dticks = SDL_GetTicks() - ticks;
    ticks += dticks;

    if (SDL_GetMouseState(&mx, &my)&SDL_BUTTON(1)) {
      drawElement(world, mode, mx, my, nx, ny, radius);
      world.flipBuffer();
    }
    nx = mx;
    ny = my;
    
    world.iterate();

    for (int r = 0; r < world.nr; r++)
      for (int c = 0; c < world.nc; c++)
	px->set(r, c, table.elements[world.at(r, c)].argb);
    px->redraw();

    osd.render(renderer, 0, 0, 2);

    SDL_RenderPresent(renderer);
    
    SDL_Delay(5);    
  }

  delete px;

  return 0;
}

