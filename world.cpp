#include "world.h"
#include <thread>
#include <cstring>
#include <cstdlib>
#include <cmath>

bool World::react(int r0, int c0, int r1, int c1) {
  ElementID result = elementAt(r0, c0).reactions[at(r1, c1)].random();
  bool ret = (result != ProbList::none);
  if (ret) set(r0, c0, result);
  return ret;
}

void World::swap(int r0, int c0, int r1, int c1) {
  buffer[r0 * nc + c0] = state[r1 * nc + c1];
  buffer[r1 * nc + c1] = state[r0 * nc + c0];
  changes[r0 * nc + c0] = changes[r1 * nc + c1] = true;
}

World::World(ElementTable* table, int nr, int nc) {
  this->table = table;
  this->nr = nr;
  this->nc = nc;
  this->border = 1;//Wall by default
  state = new ElementID [nr * nc];
  buffer = new ElementID [nr * nc];
  changes = new bool [nr * nc];
  clear();
}

World::World(const World& world) {
  state = buffer = nullptr;
  changes = nullptr;
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

void World::clear() {
  memset(buffer, 0, sizeof(ElementID) * nr * nc);
  flipBuffer();
}

void World::clearFloor() {
  memset(buffer + (nr - 1) * nc, 0, sizeof(ElementID) * nc);
  flipBuffer();
}

void World::flipBuffer() {
  memcpy(state, buffer, nr * nc * sizeof(ElementID));
  memset(changes, 0, sizeof(bool) * nr * nc);
}

void World::applyReaction() {
  int nthreads = multiThreaded? std::thread::hardware_concurrency() : 1;
  int rowsPerThread = (int)ceil((float)nr / nthreads);

  auto react_fn = [this](int r0, int r1) {
    for (int r = r0; r < r1; r++)
      for (int c = 0; c < nc; c++) {
#ifndef NO_DIAGONAL
	changed(r, c) ||
	react(r, c, r + 1, c) ||
	react(r, c, r - 1, c) ||
	react(r, c, r, c - 1) ||
	react(r, c, r, c + 1) ||
	react(r, c, r + 1, c - 1) ||
	react(r, c, r + 1, c + 1) ||
	react(r, c, r - 1, c - 1) ||
	react(r, c, r - 1, c + 1);
#else
	changed(r, c) ||
	react(r, c, r + 1, c) ||
	react(r, c, r - 1, c) ||
	react(r, c, r, c - 1) ||
	react(r, c, r, c + 1);
#endif
      }
  };

  std::vector<std::thread> T;
  for (int i = 0, r0 = 0, r1; i < nthreads; i++) {
    r1 = r0 + rowsPerThread;
    if (r1 > nr) r1 = nr;
    T.push_back(std::thread(react_fn, r0, r1));
    r0 = r1;
  }

  for (int i = 0; i < T.size(); i++) T[i].join();
  flipBuffer();
}

void World::applyDecay() {
  int nthreads = multiThreaded? std::thread::hardware_concurrency() : 1;
  int rowsPerThread = (int)ceil((float)nr / nthreads);

  auto decay_fn = [this](int r0, int r1) {
    ElementID result;
    for (int r = r0; r < r1; r++)
      for (int c = 0; c < nc; c++) {
	result = elementAt(r, c).decay.random();
	if (result != ProbList::none) buffer[r * nc + c] = result;
      }
  };

  std::vector<std::thread> T;
  for (int i = 0, r0 = 0, r1; i < nthreads; i++) {
    r1 = r0 + rowsPerThread;
    if (r1 > nr) r1 = nr;
    T.push_back(std::thread(decay_fn, r0, r1));
    r0 = r1;
  }

  for (int i = 0; i < T.size(); i++) T[i].join();
  flipBuffer();
}

inline float uniform() {return (rand() & 0x7FFF) / 32768.0;}

void World::applyShift() {
  const float pshift = 0.25;

  int nthreads = multiThreaded? std::thread::hardware_concurrency() : 1;
  int rowsPerThread = (int)ceil((float)nr / nthreads);

  auto hshift_fn = [this, pshift](int r0, int r1) {
    for (int r = r0; r < r1; r++)
      for (int c = 0; c < nc - 1; c++) {
	if (elementAt(r, c).fixed || elementAt(r, c + 1).fixed)
	  continue;
	else if (uniform() < pshift) {
	  swap(r, c, r, c + 1);
	  c++;
	}
      }
  };

  std::vector<std::thread> T;
  for (int i = 0, r0 = 0, r1; i < nthreads; i++) {
    r1 = r0 + rowsPerThread;
    if (r1 > nr) r1 = nr;
    T.push_back(std::thread(hshift_fn, r0, r1));
    r0 = r1;
  }

  for (int i = 0; i < T.size(); i++) T[i].join();
  flipBuffer();

  int colsPerThread = (int)ceil((float)nc / nthreads);

  auto vshift_fn = [this, pshift](int c0, int c1) {
    float d, d1;
    for (int r = 0; r < nr - 1; r++)
      for (int c = c0; c < c1; c++) {
	if (changed(r, c) || elementAt(r, c).fixed || elementAt(r + 1, c).fixed)
	  continue;
	d = elementAt(r, c).density;
	d1 = elementAt(r + 1, c).density;
	if (d > d1)
	  swap(r, c, r + 1, c);
	else if (d == d1 && uniform() < pshift)
	  swap(r, c, r + 1, c);
      }
  };

  T.clear();
  for (int i = 0, c0 = 0, c1; i < nthreads; i++) {
    c1 = c0 + colsPerThread;
    if (c1 > nc) c1 = nc;
    T.push_back(std::thread(vshift_fn, c0, c1));
    c0 = c1;
  }

  for (int i = 0; i < T.size(); i++) T[i].join();
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
  const char* str = "s2-0002";
  fwrite(str, 1, strlen(str) + 1, fp);
  fwrite(&nr, sizeof(int), 1, fp);
  fwrite(&nc, sizeof(int), 1, fp);
  fwrite(&border, sizeof(ElementID), 1, fp);
  fwrite(state, sizeof(ElementID), nr * nc, fp);
  fclose(fp);
}

int World::load(const char* fn) {
  FILE* fp = fopen(fn, "rb");
  if (!fp) return 1;

  const char* str = "s2-0002";
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

  fclose(fp);

  flipBuffer();

  return 0;
}

void World::drawCircle(ElementID id, int x, int y, int r) {
  for (int bw, by = -r; by <= r; by++) {
    bw = (int)(sqrt(r * r - by * by) - 0.75);
    for (int bx = -bw; bx <= bw; bx++)
      set(by + y, bx + x, id);
  }
}

void World::drawLine(ElementID id, int x0, int y0, int x1, int y1, int r) {
  float dx = x1 - x0;
  float dy = y1 - y0;
  float d = sqrt(dx * dx + dy * dy);
  if (d == 0.0) d = 1;
  dx /= d;
  dy /= d;

  for (int i = 0, x, y; i <= d; i++) {
    x = (int)(x0 + i * dx + 0.5);
    y = (int)(y0 + i * dy + 0.5);
    drawCircle(id, x, y, r);
  }
}

void World::drawRect(ElementID id, int x0, int y0, int x1, int y1) {
  int t;
  if (y0 > y1) {t = y0; y0 = y1; y1 = t;}
  if (x0 > x1) {t = x0; x0 = x1; x1 = t;}
  for (int y = y0; y <= y1; y++)
    for (int x = x0; x <= x1; x++)
      set(y, x, id);
}
