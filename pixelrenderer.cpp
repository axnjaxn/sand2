#include "pixelrenderer.h"
#include <SDL2/SDL.h>

PixelRenderer::PixelRenderer(SDL_Renderer* renderer, int w, int h) {
  nr = h;
  nc = w;
  px = new Uint32 [w * h];
  this->renderer = renderer;
  texture = SDL_CreateTexture(renderer,
			      SDL_PIXELFORMAT_ARGB8888,
			      SDL_TEXTUREACCESS_STREAMING,
			      w, h);
  clear();
}

PixelRenderer::~PixelRenderer() {
  if (px) delete [] px;
  if (texture) SDL_DestroyTexture(texture);
}

Uint32 PixelRenderer::rgba(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
  return (a << 24) | (r << 16) | (g << 8) | b;
}

void PixelRenderer::clear() {
  memset(px, 0, nr * nc * sizeof(Uint32));
}

void PixelRenderer::fill(Uint32 v) {
  for (int i = 0; i < nr * nc; i++) px[i] = v;
}

void PixelRenderer::fill(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
  Uint32 v = rgba(r, g, b, a);
  fill(v);
}

void PixelRenderer::redraw() {
  void* tex;
  int pitch;

  SDL_LockTexture(texture, NULL, &tex, &pitch);
  for (int r = 0; r < nr; r++)
    memcpy((Uint8*)tex + r * pitch, px + r * nc, nc * sizeof(Uint32));
  SDL_UnlockTexture(texture);

  SDL_RenderCopy(renderer, texture, NULL, NULL);
}
