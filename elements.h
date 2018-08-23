#ifndef _BPJ_SAND2_ELEMENTS_H
#define _BPJ_SAND2_ELEMENTS_H

extern "C" {
#include "s2struct.h"
}
#include <string>
#include <vector>

typedef unsigned int ElementID;

class ProbEntry {
public:
  ElementID id;
  float p;

  ProbEntry(ElementID id = 0, float p = 1.0) {this->id = id; this->p = p;}
};

class ProbList {
public:  
  std::vector<ProbEntry> entries;
  static const ElementID none = ~0;

  void add(const ProbEntry& entry) {entries.push_back(entry);}

  ElementID random() const;
};

typedef std::vector<ProbList> ReactionList;

class Element {
public:
  std::string name; 
  bool fixed;
  float density;
  unsigned int argb;
  ProbList decay;
  ReactionList reactions;

  Element(const std::string& name);
};

class ElementTable {
public:
  std::vector<Element> elements;
  std::vector<ElementID> menu;
  
  ElementTable(Sand2Spec* spec);
  int getIndex(const std::string& name);
};

#endif
