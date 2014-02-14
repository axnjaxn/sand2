#ifndef _BPJ_OSD_H
#define _BPJ_OSD_H

#include "bbc_font.h"
#include <string>

class OSD {
public:
  static BBC_Font font;

  std::string text;
  Uint32 start, end;
  Uint8 r, g, b;
  bool fade, vanish;

  OSD();

  void setText(const std::string& text, Uint32 t);
  void setColor(Uint8 r, Uint8 g, Uint8 b);
  void enableFade(bool enable = 1);
  void enableVanish(bool enable = 1);

  bool isVisible() const;
  void render(SDL_Renderer* render, int x, int y, int scale = 1) const;
};

#endif
