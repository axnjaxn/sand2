#ifndef _BPJ_PIXELRENDERER_H
#define _BPJ_PIXELRENDERER_H

#include <SDL2/SDL.h>

class PixelRenderer {
private:
  PixelRenderer(const PixelRenderer&) { }
  PixelRenderer& operator=(const PixelRenderer&) {return *this;}

protected:
  int nr, nc;
  SDL_Renderer* renderer;
  SDL_Texture* texture;
  Uint32* px;

public:
  PixelRenderer(SDL_Renderer* renderer, int w, int h);
  ~PixelRenderer();

  static Uint32 rgba(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 0xFF);

  inline Uint32& at(int r, int c) {return px[r * nc + c];}
  inline Uint32 at(int r, int c) const {return px[r * nc + c];}
  inline Uint8 at(int r, int c, int ch) const {return (px[r * nc + c] >> (8 * ch)) & 0xFF;}

  inline void set(int r, int c, Uint32 v) {at(r, c) = v;}
  void set(int r, int c, Uint8 R, Uint8 G, Uint8 B, Uint8 A = 0xFF) {at(r, c) = rgba(R, G, B, A);}

  void clear();

  void fill(Uint32 v);
  void fill(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 0xFF);

  inline int width() const {return nc;}
  inline int height() const {return nr;}

  void redraw();//To be called to update the texture on the back end
};

#endif
