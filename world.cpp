#include "world.h"
#include <cstring>
#include <cstdlib>

void World::react(int r0, int c0, int r1, int c1) {
  ElementID result = elementAt(r0, c0).reactions[at(r1, c1)].random();
  if (result != ProbList::none) set(r0, c0, result);
}

void World::swap(int r0, int c0, int r1, int c1) {
  ElementID t = at(r0, c0);
  set(r0, c0, at(r1, c1));
  set(r1, c1, t);  
}

World::World(ElementTable* table, int nr, int nc) {
  this->table = table;
  this->nr = nr;
  this->nc = nc;
  this->border = 1;//Wall by default
  state = new ElementID [nr * nc];
  buffer = new ElementID [nr * nc];
  changes = new bool [nr * nc];
  memset(state, 0, sizeof(ElementID) * nr * nc);
  memset(buffer, 0, sizeof(ElementID) * nr * nc);
  memset(changes, 0, sizeof(bool) * nr * nc);
}

World::World(const World& world) {
  state = buffer = NULL;
  changes = NULL;
  *this = world;
}

World::~World() {
  if (state) delete [] state;
  if (buffer) delete [] buffer;
  if (changes) delete [] changes;
}

World& World::operator=(const World& world) {
  if (state) delete [] state;
  if (buffer) delete [] buffer;
  if (changes) delete [] changes;

  table = world.table;
  border = world.border;
  nr = world.nr;
  nc = world.nc;
  state = new ElementID [nr * nc];
  buffer = new ElementID [nr * nc];
  changes = new bool [nr * nc];
  memcpy(state, world.state, nr * nc * sizeof(ElementID));
  memcpy(buffer, world.buffer, nr * nc * sizeof(ElementID));
  memcpy(changes, world.changes, nr * nc * sizeof(bool));

  return *this;
}

bool World::changed(int r, int c) const {
  return changes[r * nc + c];
}

ElementID World::at(int r, int c) const { 
  if (r < 0 || r >= nr || c < 0 || c >= nc) return border;
  return state[r * nc + c];
}

void World::set(int r, int c, ElementID id) {
  if (r < 0 || r >= nr || c < 0 || c >= nc) return;
  buffer[r * nc + c] = id;
  changes[r * nc + c] = 1;
}

const Element& World::elementAt(int r, int c) const {
  return table->elements[at(r, c)];
}

void World::flipBuffer() {
  memcpy(state, buffer, nr * nc * sizeof(ElementID));
  memset(changes, 0, sizeof(bool) * nr * nc);
}

void World::applyReaction() {
  for (int r = 0; r < nr; r++)
    for (int c = 0; c < nc; c++) {
      if (changed(r, c)) continue;
      react(r, c, r + 1, c);
      if (changed(r, c)) continue;
      react(r, c, r - 1, c);
      if (changed(r, c)) continue;
      react(r, c, r, c - 1);
      if (changed(r, c)) continue;
      react(r, c, r, c + 1);
      if (changed(r, c)) continue;
      react(r, c, r + 1, c - 1);
      if (changed(r, c)) continue;
      react(r, c, r + 1, c + 1);
      if (changed(r, c)) continue;
      react(r, c, r - 1, c - 1);
      if (changed(r, c)) continue;
      react(r, c, r - 1, c + 1);
    }

  flipBuffer();
}

void World::applyDecay() {
  ElementID result;
  for (int r = 0; r < nr; r++)
    for (int c = 0; c < nc; c++) {
      if (changed(r, c)) continue;
      result = elementAt(r, c).decay.random();
      if (result != ProbList::none) set(r, c, result);
    }

  flipBuffer();
}

inline float uniform() {return (rand() & 0x7FFF) / 32768.0;}

void World::applyShift() {
  const float pshift = 0.25;
  
  for (int r = 0; r < nr - 1; r++)
    for (int c = 0; c < nc; c++) {
      if (changed(r, c) || elementAt(r, c).fixed || changed(r + 1, c) || elementAt(r + 1, c).fixed) 
	continue;
      else if (elementAt(r, c).density >= elementAt(r + 1, c).density)
	swap(r, c, r + 1, c);
    }

  flipBuffer();

  for (int r = 0; r < nr; r++)
    for (int c = 0; c < nc - 1; c++) {
      if (changed(r, c) || elementAt(r, c).fixed || changed(r, c + 1) || elementAt(r, c + 1).fixed) 
	continue;
      else if (uniform() < pshift)
	swap(r, c, r, c + 1);
    }

  flipBuffer();
}

void World::iterate() {
  applyReaction();
  applyDecay();
  applyShift();
}
