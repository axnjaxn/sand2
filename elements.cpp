#include "elements.h"
#include <cstdlib>
#include <exception>

inline float uniform() {return (rand() & 0x7FFF) / 32768.0;}

ElementID ProbList::random() const {
  if (entries.empty()) return none;
  
  float f = uniform();
  for (int i = 0; i < entries.size(); i++) {
    if (f < entries[i].p) return entries[i].id;
    f -= entries[i].p;
  }
  return none;
}

Element::Element(const std::string& name) {
  this->name = name;
  fixed = 1;
  density = 0.0;
}

ElementTable::ElementTable(Sand2Spec* spec) {
  std::string name;
  int i, j, k;

  /*
   * Step 1: Get the element names (so that they can be bound)
   */
  elements.push_back(Element("Air"));
  elements.push_back(Element("Wall"));
  for (S2List* lst = spec->properties; lst; lst = lst->next) {
    name = std::string(lst->str);
    
    bool breakflag = 0;
    for (int i = 0; i < elements.size(); i++)
      if (name == elements[i].name) {
	breakflag = 1;
	break;
      }
    if (!breakflag)
      elements.push_back(Element(name));  
  }

  /*
   * Step 2: Set up properties for each element
   */
  for (S2List* node = spec->properties; node; node = node->next) {
    name = std::string(node->str);

    //Find element we're defining
    for (i = 0; i < elements.size(); i++)
      if (name == elements[i].name) break;
    
    elements[i].fixed = node->prop->fixed;
    elements[i].density = node->prop->density;
    elements[i].pressure = node->prop->pressure;
    elements[i].argb = node->prop->argb;
    elements[i].reactions.resize(elements.size());

    for (S2List* rnode = node->prop->reactions; rnode; rnode = rnode->next) {
      //Element being reacted to
      name = std::string(rnode->str);
      for (j = 0; j < elements.size(); j++)
	if (name == elements[j].name) break;
      if (j == elements.size()) continue;
 
      for (S2List* rnode_p = rnode->lst; rnode_p; rnode_p = rnode_p->next) {
	//Result element
	name = std::string(rnode_p->str);
	for (k = 0; k < elements.size(); k++)
	  if (name == elements[k].name) break;
	if (k == elements.size()) continue;

	elements[i].reactions[j].add(ProbEntry((ElementID)k, rnode_p->p));
      }
    }

    for (S2List* dnode = node->prop->decay; dnode; dnode = dnode->next) {
      //Element to decay to
      name = std::string(dnode->str);
      for (j = 0; j < elements.size(); j++)
	if (name == elements[j].name) break;
      if (j == elements.size()) continue;

      elements[i].decay.add(ProbEntry((ElementID)j, dnode->p));
    }
  }

  /*
   * Now make the menu
   */
  if (!spec->menu)
    for (i = 0; i < elements.size(); i++)
      menu.push_back((ElementID)i);
  else {
    for (S2List* node = spec->menu; node; node = node->next) {
      i = getIndex(node->str);
      if (i == elements.size()) continue;
      menu.push_back((ElementID)i);
    }
  }
}

int ElementTable::getIndex(const std::string& name) {
  int i;
  for (i = 0; i < elements.size(); i++)
    if (name == elements[i].name) break;
  return i;
}
