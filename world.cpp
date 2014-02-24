#include "world.h"
#include <cstring>
#include <cstdlib>

void World::react(int r0, int c0, int r1, int c1) {
  ElementID result = elementAt(r0, c0).reactions[at(r1, c1)].random();
  if (result != ProbList::none) set(r0, c0, result);
}

void World::swap(int r0, int c0, int r1, int c1) {
  ElementID t = at(r0, c0);
  int p = pressureAt(r0, c0);
  set(r0, c0, at(r1, c1), pressureAt(r1, c1));
  set(r1, c1, t, p);
}

World::World(ElementTable* table, int nr, int nc) {
  this->table = table;
  this->nr = nr;
  this->nc = nc;
  this->border = 1;//Wall by default
  state = new ElementID [nr * nc];
  buffer = new ElementID [nr * nc];
  pressure = new int [nr * nc];
  pbuffer = new int [nr * nc];
  changes = new bool [nr * nc];
  clear();
}

World::World(const World& world) {
  state = buffer = NULL;
  changes = NULL;
  *this = world;
}

World::~World() {
  if (state) delete [] state;
  if (buffer) delete [] buffer;
  if (pressure) delete [] pressure;
  if (pbuffer) delete [] pbuffer;
  if (changes) delete [] changes;
}

World& World::operator=(const World& world) {
  if (state) delete [] state;
  if (buffer) delete [] buffer;
  if (pressure) delete [] pressure;
  if (pbuffer) delete [] pbuffer;
  if (changes) delete [] changes;

  table = world.table;
  border = world.border;
  nr = world.nr;
  nc = world.nc;
  state = new ElementID [nr * nc];
  buffer = new ElementID [nr * nc];
  pressure = new int [nr * nc];
  pbuffer = new int [nr * nc];
  changes = new bool [nr * nc];
  memcpy(state, world.state, nr * nc * sizeof(ElementID));
  memcpy(buffer, world.buffer, nr * nc * sizeof(ElementID));
  memcpy(pressure, world.pressure, nr * nc * sizeof(int));
  memcpy(pbuffer, world.pbuffer, nr * nc * sizeof(int));
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
  pbuffer[r * nc + c] = elementAt(r, c).pressure;
  changes[r * nc + c] = 1;
}

void World::set(int r, int c, ElementID id, int pressure) {
  if (r < 0 || r >= nr || c < 0 || c >= nc) return;
  buffer[r * nc + c] = id;
  pbuffer[r * nc + c] = pressure;
  changes[r * nc + c] = 1;
}

const Element& World::elementAt(int r, int c) const {
  return table->elements[at(r, c)];
}

int World::pressureAt(int r, int c) const {
  return pressure[r * nc + c];
}

void World::clear() {
  memset(buffer, 0, sizeof(ElementID) * nr * nc);
  memset(pbuffer, 0, sizeof(int) * nr * nc);
  flipBuffer();
}

void World::clearFloor() {
  memset(buffer + (nr - 1) * nc, 0, sizeof(ElementID) * nc);
  memset(pbuffer + (nr - 1) * nc, 0, sizeof(int) * nc);
  flipBuffer();
}

void World::flipBuffer() {
  memcpy(state, buffer, nr * nc * sizeof(ElementID));
  memcpy(pressure, pbuffer, nr * nc * sizeof(int));
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
  
  for (int r = 0; r < nr; r++)
    for (int c = 0; c < nc - 1; c++) {
      if (changed(r, c) || elementAt(r, c).fixed || changed(r, c + 1) || elementAt(r, c + 1).fixed) 
	continue;
      else if (uniform() < pshift)
	swap(r, c, r, c + 1);
    }

  flipBuffer();

  for (int r = 0; r < nr - 1; r++)
    for (int c = 0; c < nc; c++) {
      if (changed(r, c) || elementAt(r, c).fixed || changed(r + 1, c) || elementAt(r + 1, c).fixed) 
	continue;
      else if (elementAt(r, c).density > elementAt(r + 1, c).density)
	swap(r, c, r + 1, c);
      else if (elementAt(r, c).density == elementAt(r + 1, c).density && uniform() < pshift)
	swap(r, c, r + 1, c);
    }

  flipBuffer();
}

void World::iterate() {
  applyDecay();
  applyReaction();
  applyShift();
}

#include <cstdio>

void World::save(const char* fn) const {
  FILE* fp = fopen(fn, "wb");
  const char* str = "s2-0001";
  fwrite(str, 1, strlen(str) + 1, fp);
  fwrite(&nr, sizeof(int), 1, fp);
  fwrite(&nc, sizeof(int), 1, fp);
  fwrite(&border, sizeof(ElementID), 1, fp);
  fwrite(state, sizeof(ElementID), nr * nc, fp);
  fwrite(pressure, sizeof(int), nr * nc, fp);
  fclose(fp);
}

int World::load(const char* fn) {
  FILE* fp = fopen(fn, "rb");
  if (!fp) return 1;

  const char* str = "s2-0001";
  char checkstr[8];
  fread(checkstr, 1, 8, fp);
  if (strcmp(str, checkstr)) {
    fclose(fp); 
    return 2;
  }

  int nr, nc;
  fread(&nr, sizeof(int), 1, fp);
  fread(&nc, sizeof(int), 1, fp);
  if (nr != this->nr || nc != this->nc) {
    fclose(fp);
    return 3;
  }

  fread(&border, sizeof(ElementID), 1, fp);
  fread(buffer, sizeof(ElementID), nr * nc, fp);
  fread(pbuffer, sizeof(int), nr * nc, fp);

  fclose(fp);

  flipBuffer();

  return 0;
}
