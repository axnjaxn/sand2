#ifndef _BPJ_SAND2_RANDOMIZER_H
#define _BPJ_SAND2_RANDOMIZER_H

#include <cstdlib>
#include <ctime>

class Randomizer {
public:
  static void seed() {srand(time(NULL));}
  static void seed(unsigned int s) {srand(s);}
  static int rand() {return rand();}
  static float uniform() {return (rand() & 0x7FFF) / 32768.0;}
};

#endif
