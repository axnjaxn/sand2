#include "world.h"
#include <cstring>

void World::react(int r0, int c0, int r1, int c1) {
  ElementID result = table->elements[at(r0, c0)].reactions[at(r1, c1)].random();
  if (result != ProbList::none) set(r0, c0, result);
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
  buffer[r * nc + c] = id;
  changes[r * nc + c] = 1;
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

void World::applyDecay() { }

void World::applyShift() { }

void World::iterate() { }
