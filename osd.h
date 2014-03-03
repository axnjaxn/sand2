/*
 * osd.h
 * On Screen Display by Brian Jackson
 * Developed as part of BlameGear: Brian's Lame Game Gear Emulator
 * Last updated: 3 March 2014
 */

#ifndef _BPJ_OSD_H
#define _BPJ_OSD_H

#include "bbc_font.h"
#include <string>

class OSD {
public:
  static BBC_Font font;

  std::string text;
  Uint32 start, end, fadetime;
  Uint8 r, g, b;
  bool fade, vanish, enabled;

  OSD();
  
  virtual void setText(const std::string& text);
  virtual void setTextf(const char* str, ...);
  void setColor(Uint8 r, Uint8 g, Uint8 b);
  void setTime(Uint32 fadetime);
  void setVisible(bool enable = 1);
  void enableFade(bool enable = 1);
  void enableVanish(bool enable = 1);

  bool isVisible() const;
  
  virtual void render(SDL_Renderer* render, int x, int y, int scale = 1) const;
};

#endif
