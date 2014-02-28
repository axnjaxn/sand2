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

#ifdef DEBUG
void readMacro(World& world, const char* macro) {
  FILE* fp = fopen(macro, "r");
  if (!fp) return;
  
  ElementTable* table = world.table;

  char buf[256];
  while (!feof(fp)) {
    fscanf(fp, "%s", buf);
    if (!strcmp(buf, "done")) break;
    else if (!strcmp(buf, "rect")) {
      ElementID id;
      int r0, c0, r1, c1;
      fscanf(fp, "%s %d %d %d %d", buf, &r0, &c0, &r1, &c1);
      id = table->getIndex(buf);
      world.drawRect(id, c0, r0, c1, r1);
    }
    else if (!strcmp(buf, "cells")) {
      int r, c;
      fscanf(fp, "%d %d", &r, &c);

      ElementID id;
      fscanf(fp, "%s", buf);
      while (strcmp(buf, "done")) {
	id = table->getIndex(buf);
	fscanf(fp, "%s", buf);
	world.set(r, c, id);
	if (++c > world.nc) {
	  r += c / world.nc;
	  c = c % world.nc;
	}
      }
    }
  }

  fclose(fp);

  world.flipBuffer();
}

void chooseMacro(World& world) {
  char fn[256];
  printf("Enter a macro filename: ");
  fflush(0);
  scanf("%s", fn);
  readMacro(world, fn);
  printf("Done.\n");
}
#endif

int main(int argc, char* argv[]) {
  /*
   * CLI args and global configuration
   */
  int nr = 400, nc = 640;//Number of rows (height) and columns (width)
  int sc = 2; //Display scale (in pixels)
  std::string element_filename = "elements.dat";
  bool setfile = 0, no_osd = 0;

  argc--;
  argv++;
  for (int i = 0; i < argc;) {
    if (!strcmp(argv[i], "--rows")) {
      sscanf(argv[i + 1], "%d", &nr);
      i += 2;
    }
    else if (!strcmp(argv[i], "--cols")) {
      sscanf(argv[i + 1], "%d", &nc);
      i += 2;
    }
    else if (!strcmp(argv[i], "--scale")) {
      sscanf(argv[i + 1], "%d", &sc);
      i += 2;
    }
    else if (!strcmp(argv[i], "--no-osd")) {
      no_osd = 1;
      i++;
    }
    else if (!setfile) {
      element_filename = argv[i++];
      setfile = 1;
    }
    else {
      printf("Invalid command-line argument: \"%s\".\n", argv[i]);
      return 1;
    }
  }
  
  extern FILE* yyin;
  yyin = fopen(element_filename.c_str(), "r");

  /*
   * Parsing element table
   */

  srand(time(NULL));

  extern Sand2Spec* gspec;
  yyparse();

  ElementTable table(gspec);
  destroySpec(gspec);

  /*
   * Initialize world
   */

  World world(&table, nr, nc);

  /*
   * Initialize SDL 2.0
   */

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("Fatal error initializing SDL");
    return 1;
  }
  atexit(SDL_Quit);
  
  SDL_Window* window = SDL_CreateWindow(TITLE,
					SDL_WINDOWPOS_CENTERED,
					SDL_WINDOWPOS_CENTERED,
					nc * sc, nr * sc,
					0);
  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

  /*
   * Set up PixelRender and On Screen Display
   */

  PixelRenderer* px = new PixelRenderer(renderer, nc, nr);
  SDL_RenderSetLogicalSize(renderer, nc, nr);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderClear(renderer);

  OSD osd;
  if (no_osd) osd.setVisible(0);
  osd.enableFade();
  osd.setTime(5000);
  osd.setText("Arrows to change element / size");
  osd.setTime(3000);
  const int osd_time = 2000;
  
  /* 
   * Set up draw tools and begin main loop
   */

  int radius = 3, mx, my, nx, ny;
  int modeIndex = (table.menu.size() > 2)? 2 : table.menu.size() - 1;
  ElementID mode = table.menu[modeIndex];
  int drawmode = 0;
  SDL_Event event;
  bool exitflag = 0, paused = 0, floor = 1, modeflag = 0, slowmo = 0;
  int mousedown = 0;
  Uint32 ticks = 0, dticks;
  while (!exitflag) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) exitflag = 1;
      else if (event.type == SDL_MOUSEBUTTONDOWN) {
	mx = nx = event.button.x;
	my = ny = event.button.y;
	if (event.button.button == SDL_BUTTON_LEFT) 
	  mousedown = 1; 
	else if (event.button.button == SDL_BUTTON_RIGHT) 
	  mousedown = 2;
      }
      else if (event.type == SDL_MOUSEBUTTONUP) {
	if (mousedown == 2) {
	  if (drawmode) world.drawRect(mode, mx, my, nx, ny);
	  else world.drawLine(mode, mx, my, nx, ny, radius);
	  world.flipBuffer();
	}
	mousedown = 0;
      }
      else if (event.type == SDL_MOUSEMOTION) {
	mx = event.motion.x;
	my = event.motion.y;
      }
      else if (event.type == SDL_KEYDOWN) {
	switch (event.key.keysym.sym) {
	default: break;
	case SDLK_F2: world.save("sand2.sav"); osd.setText("World saved to sand2.sav!"); break;
	case SDLK_F3: 
	  if (world.load("sand2.sav")) osd.setText("Could not load sand2.sav!");
	  else osd.setText("World loaded from sand2.sav!");
	  break;	  
	case SDLK_ESCAPE: exitflag = 1; break;
	case SDLK_BACKSPACE: world.clear(); osd.setText("World cleared!"); break;
	case SDLK_SPACE: 
	  paused = !paused;
	  if (paused) 
	    osd.setText("Paused");
	  else
	    osd.setText("Resumed");
	  break;
	case SDLK_b:
	  floor = !floor;
	  if (floor) osd.setText("Bottom boundary on");
	  else osd.setText("Bottom boundary off");
	  break;
	case SDLK_f:
	  osd.setTextf("%.1f FPS", 1000.0 / dticks);
	  break;
#ifdef DEBUG
	case SDLK_m:
	  chooseMacro(world);
	  paused = 1;
	  SDL_RenderPresent(renderer);
	  break;
#endif
	case SDLK_s:
	  slowmo = !slowmo;
	  osd.setTextf("Slowmo: %s", (slowmo)? "on" : "off");
	  break;
	case SDLK_t:
	  paused = 1;
	  world.iterate();
	  if (!floor) world.clearFloor();
	  break;
	case SDLK_UP:
	  radius++; 
	  osd.setTextf("Radius: %d", radius); 
	  break;
	case SDLK_DOWN: 
	  radius--; 
	  osd.setTextf("Radius: %d", radius);
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
	case SDLK_LSHIFT: drawmode = !drawmode; break;
	}
      }
    }

    dticks = SDL_GetTicks() - ticks;
    ticks += dticks;

    if (modeflag) {
      modeflag = 0;
      mode = table.menu[modeIndex];
      osd.setText("Element: " + table.elements[mode].name);
    }

    if (mousedown == 1) {
      if (SDL_GetModState() & KMOD_CTRL)
	world.set(my, mx, mode);
      else
	world.drawLine(mode, mx, my, nx, ny, radius);
      world.flipBuffer();
      nx = mx;
      ny = my;
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
      if (!drawmode) {
	SDL_RenderDrawLine(renderer, mx, my, nx, ny);
	SDL_RenderDrawLine(renderer, mx - radius, my, mx + radius, my);
	SDL_RenderDrawLine(renderer, mx, my - radius, mx, my + radius);
      }
      else {
	SDL_RenderDrawLine(renderer, mx, my, nx, my);
	SDL_RenderDrawLine(renderer, nx, my, nx, ny);
	SDL_RenderDrawLine(renderer, nx, ny, mx, ny);
	SDL_RenderDrawLine(renderer, mx, ny, mx, my);
      }
    }

    SDL_RenderPresent(renderer);

    if (slowmo) SDL_Delay(100);
    else SDL_Delay(5);
  }

  delete px;

  return 0;
}

