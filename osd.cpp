/*
 * osd.cpp
 * On Screen Display by Brian Jackson
 * Developed as part of BlameGear: Brian's Lame Game Gear Emulator
 * Last updated: 3 March 2014
 */

#include "osd.h"

BBC_Font OSD::font;

OSD::OSD() {start = end = 0; setColor(255, 255, 255); setTime(3000); fade = vanish = 0;}

void OSD::setText(const std::string& text) {
  this->text = text;
  start = SDL_GetTicks();
  end = start + fadetime;
}

void OSD::setTextf(const char* str, ...) {
  va_list args;
  va_start(args, str);
  char buf[1024];
  vsprintf(buf, str, args);
  setText(buf);
}

void OSD::setColor(Uint8 r, Uint8 g, Uint8 b) {this->r = r; this->g = g; this->b = b;}

void OSD::setTime(Uint32 fadetime) {this->fadetime = fadetime;}

void OSD::setVisible(bool enable) {enabled = enable;}

void OSD::enableFade(bool enable) {fade = enable;}

void OSD::enableVanish(bool enable) {vanish = enable;}

bool OSD::isVisible() const {
  return SDL_GetTicks() < end;
}

void OSD::render(SDL_Renderer* render, int x, int y, int scale) const {
  if (!enabled) return;

  Uint32 t = SDL_GetTicks();
  if (t >= end) return;

  float f = 1.0 - (float)(t - start) / (end - start);
  if (fade)
    SDL_SetRenderDrawColor(render, r, g, b, (Uint8)(255.0 * f));
  else if (!vanish)
    SDL_SetRenderDrawColor(render, (Uint8)(f * r), Uint8(f * g), Uint8 (f * b), 255);
  else 
    SDL_SetRenderDrawColor(render, r, g, b, 255);

  SDL_Rect rect;
  rect.x = x;
  rect.y = y;

  font.renderString(text.c_str(), render, rect, scale);
}
