#include "randomizer.h"
#include "elements.h"

ElementID ProbList::random() {
  float f = Randomizer::uniform();
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
  /*
   * Step 1: Get the element names (so that they can be bound)
   */
  elements.push_back(Element("Air"));
  elements.push_back(Element("Wall"));
  for (S2List* lst = spec->properties; lst; lst = lst->next) {
    std::string name(lst->str);
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
  for (S2List* node = spec->properties; node; node = node->next) {
    Element element(node->str);
    element.fixed = node->prop->fixed;
    element.density = node->prop->density;
    element.argb = node->prop->argb;
    for (S2List* rnode = spec->prop->reactions; rnode; rnode = rnode->next) {
      element.
    }
  }
  */ 
}
