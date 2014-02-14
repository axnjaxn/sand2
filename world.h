#ifndef _BPJ_SAND2_WORLD_H
#define _BPJ_SAND2_WORLD_H

#include "elements.h"

class World {
protected:
  void react(int r0, int c0, int r1, int c1);
  void swap(int r0, int c0, int r1, int c1); 

public:
  ElementTable* table;
  ElementID border;
  int nr, nc;
  ElementID *state, *buffer;
  bool* changes;

  World(ElementTable* table, int nr, int nc);
  World(const World& world);
  ~World();
  
  World& operator=(const World& world);

  bool changed(int r, int c) const;
  ElementID at(int r, int c) const;
  void set(int r, int c, ElementID id);
  const Element& elementAt(int r, int c) const;

  void clear(ElementID fill = 0);
  
  void flipBuffer();

  void applyReaction();
  void applyDecay();
  void applyShift();
  
  void iterate();
  void draw() { }//uf
};

#endif
