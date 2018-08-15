#ifndef _BPJ_SAND2_WORLD_H
#define _BPJ_SAND2_WORLD_H

#include "elements.h"

class World {
protected:
  void react(int r0, int c0, int r1, int c1);
  void decompress(int r, int c);
  void swap(int r0, int c0, int r1, int c1);

  void applyReaction(int rstart, int rend);
  void applyReaction();
  void applyDecay(int rstart, int rend);
  void applyDecay();
  void applyShift();

public:
  ElementTable* table;
  ElementID border;
  int nr, nc;
  ElementID *state, *buffer;
  float *pressure, *pbuffer;
  bool* changes;

  World(ElementTable* table, int nr, int nc);
  World(const World& world);
  ~World();
  
  World& operator=(const World& world);

  bool changed(int r, int c) const;
  ElementID at(int r, int c) const;
  void set(int r, int c, ElementID id);
  void set(int r, int c, ElementID id, float pressure);
  const Element& elementAt(int r, int c) const;
  float pressureAt(int r, int c) const;

  void clear();
  void clearFloor();
  
  void flipBuffer();

  void iterate();

  void save(const char* fn) const;
  int load(const char* fn); //Returns 0 on successful load

  void drawCircle(ElementID id, int x, int y, int r);
  void drawLine(ElementID id, int x0, int y0, int x1, int y1, int r);
  void drawRect(ElementID id, int x0, int y0, int x1, int y1);
};

#endif
