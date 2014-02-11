#ifndef _BPJ_SAND2_CONTEXT_H
#define _BPJ_SAND2_CONTEXT_H

#include "randomizer.h"
#include <string>
#include <vector>
#include <list>

typedef unsigned int ElementID;

class ProbEntry {
public:
  ElementID id;
  float p;

  ProbEntry(ElementID id = 0, float p = 1.0) {
    this->id = id; this->p = p;
  }
};

class ProbList {
public:  
  std::vector<ProbEntry> entries;
  static const ElementID none = ~0;

  void add(const ProbEntry& entry) {
    entries.push_back(entry);
  }

  ElementID random() {
    float f = Randomizer::uniform();
    for (int i = 0; i < entries.size(); i++) {
      if (f < entries[i].p) return entries[i].id;
      f -= entries[i].p;
    }
    return none;
  }
};

typedef std::vector<ProbList> ReactionList;

class Element {
public:
  ElementID id;
  std::string name; 
  bool fixed;
  float density;
  ProbList decay;
  ProbList spawn;
  ReactionList reactions;

  Element(int id, std::string name) {
    this->id = id;
    this->name = name;
    fixed = 1;
    density = 0.0;
  }
};

class sand2_ctx {
public:
  std::string filename;
  std::vector<Element> elements;

  sand2_ctx(const std::string& filename) {
    this->filename = filename;
    elements.push_back(Element(0, "Air"));
    elements[0].fixed = 0;
    elements.push_back(Element(1, "Wall"));
  }

  void addElement(std::string name) {
    elements.push_back(Element(elements.size(), name));
  }

  void addElements(std::list<std::string> names) {
    for (std::list<std::string>::iterator it = names.begin(); it != names.end(); it++) {
      addElement(*it);
    }
  }

  void print() {
    for (int i = 0; i < elements.size(); i++)
      printf("%d: %s\n", elements[i].id, elements[i].name.c_str());
  }
};

#endif
