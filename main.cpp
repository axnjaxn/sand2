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
  const int w = 640, h = 400, sc = 2;

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
					w * sc, h * sc,
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
  int modeIndex = 0;
  ElementID mode = table.menu[0];

  SDL_Event event;
  bool exitflag = 0, paused = 0, floor = 1, modeflag = 0;
  int mousedown = 0;
  Uint32 ticks = 0, dticks;
  while (!exitflag) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) exitflag = 1;
      else if (event.type == SDL_MOUSEBUTTONDOWN) {
	SDL_GetMouseState(&nx, &ny);
	if (event.button.button == SDL_BUTTON_LEFT) 
	  mousedown = 1; 
	else if (event.button.button == SDL_BUTTON_RIGHT) 
	  mousedown = 2;
      }
      else if (event.type == SDL_MOUSEBUTTONUP) {
	if (mousedown == 2) {
	  drawElement(world, mode, mx / sc, my / sc, nx / sc, ny / sc, radius);
	  world.flipBuffer();
	}
	mousedown = 0;
      }
      else if (event.type == SDL_KEYDOWN) {
	switch (event.key.keysym.sym) {
	default: break;
	case SDLK_F2: world.save("sand2.sav"); osd.setText("World saved to sand2.sav!", osd_time); break;
	case SDLK_F3: 
	  if (world.load("sand2.sav")) osd.setText("Could not load sand2.sav!", osd_time);
	  else osd.setText("World loaded from sand2.sav!", osd_time);
	  break;	  
	case SDLK_ESCAPE: exitflag = 1; break;
	case SDLK_BACKSPACE: world.clear(); osd.setText("World cleared!", osd_time); break;
	case SDLK_SPACE: 
	  paused = !paused;
	  if (paused) 
	    osd.setText("Paused", osd_time);
	  else
	    osd.setText("Resumed", osd_time);
	  break;
	case SDLK_b:
	  floor = !floor;
	  if (floor) osd.setText("Bottom boundary on", osd_time);
	  else osd.setText("Bottom boundary off", osd_time);
	  break;
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
	  modeIndex = (modeIndex + table.menu.size() - 1) % table.menu.size();
	  modeflag = 1;
	  break;
	case SDLK_RIGHT:
	  modeIndex = (modeIndex + 1) % table.menu.size();
	  modeflag = 1;
	  break;
	case SDLK_0: modeIndex = 0; modeflag = 1; break;
	case SDLK_1: modeIndex = 1; modeflag = 1; break;
	case SDLK_2: if (table.elements.size() >= 2) modeIndex = 2; modeflag = 1; break;
	case SDLK_3: if (table.elements.size() >= 3) modeIndex = 3; modeflag = 1; break;
	case SDLK_4: if (table.elements.size() >= 4) modeIndex = 4; modeflag = 1; break;
	case SDLK_5: if (table.elements.size() >= 5) modeIndex = 5; modeflag = 1; break;
	case SDLK_6: if (table.elements.size() >= 6) modeIndex = 6; modeflag = 1; break;
	case SDLK_7: if (table.elements.size() >= 7) modeIndex = 7; modeflag = 1; break;
	case SDLK_8: if (table.elements.size() >= 8) modeIndex = 8; modeflag = 1; break;
	case SDLK_9: if (table.elements.size() >= 9) modeIndex = 9; modeflag = 1; break;
	}
      }
    }

    dticks = SDL_GetTicks() - ticks;
    ticks += dticks;

    if (modeflag) {
      modeflag = 0;
      mode = table.menu[modeIndex];
      osd.setText("Element: " + table.elements[mode].name, osd_time);
    }

    if (mousedown == 1) {
      SDL_GetMouseState(&mx, &my);
      drawElement(world, mode, mx / sc, my / sc, nx / sc, ny / sc, radius);
      world.flipBuffer();
      nx = mx;
      ny = my;
    } 
    else if (mousedown == 2) {
      SDL_GetMouseState(&mx, &my);
    }

    if (!paused) {
      world.iterate();
      if (!floor) world.clearFloor();
    }

    for (int r = 0; r < world.nr; r++)
      for (int c = 0; c < world.nc; c++)
	px->set(r, c, table.elements[world.at(r, c)].argb);
    px->redraw();

    osd.render(renderer, 0, 0, 2);
    if (mousedown == 2) {
      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
      SDL_RenderDrawLine(renderer, mx / sc, my / sc, nx / sc, ny / sc);
      SDL_RenderDrawLine(renderer, mx / sc - 5, my / sc, mx / sc + 5, my / sc);
      SDL_RenderDrawLine(renderer, mx / sc, my / sc - 5, mx / sc, my / sc + 5);
    }

    SDL_RenderPresent(renderer);
    
    SDL_Delay(5);    
  }

  delete px;

  return 0;
}

