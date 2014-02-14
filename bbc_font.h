/*
 * To do:
 * Decide whether I'm using SDL_Surfaces to store the data or not
 * - If this is the case, the unsigned char data should not even really be used
 * - This could actually be used to set up fonts with and without backgrounds, for example
 * Optimize the heck out of this sucker 
 */

#ifndef _BPJ_BBC_FONT
#define _BPJ_BBC_FONT

#include <SDL2/SDL.h>

class BBC_Font {
public:
  unsigned char data[128 * 8];

  BBC_Font();

  SDL_Surface* getFont(Uint32 fg = ~0);

  //Deprecated
  void drawChar(char ch, SDL_Surface* target, SDL_Rect rect, Uint32 fg = ~0) const;
  void drawString(const char* str, SDL_Surface* target, SDL_Rect rect, Uint32 fg = ~0) const;
  void drawCharScaled(char ch, SDL_Surface* target, SDL_Rect rect, int scale, Uint32 fg = ~0) const;
  void drawStringScaled(const char* str, SDL_Surface* target, SDL_Rect rect, int scale, Uint32 fg = ~0) const;

  void renderChar(char ch, SDL_Renderer* render, SDL_Rect rect, int scale = 1) const;
  void renderString(const char* str, SDL_Renderer* render, SDL_Rect rect, int scale = 1) const;
};

#endif
