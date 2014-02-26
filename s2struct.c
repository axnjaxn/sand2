#include "s2struct.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

S2List* cons(const char* str, S2List* next) {
    S2List* sl = malloc(sizeof(S2List));
    strcpy(sl->str, str);
    sl->next = next;
    return sl;
}

void destroyList(S2List* sl) {
    S2List* next;
    while (sl) {
	next = sl->next;
	free(sl);
	sl = next;
    }
}

S2List* addProb(const char* str, float p, S2List* next) {
    S2List* sl = cons(str, next);
    sl->p = p;
    return sl;
}

S2List* addList(const char* str, S2List* lst, S2List* next) {
    S2List* sl = cons(str, next);
    sl->lst = lst;
    return sl;
}

S2List* addProperties(const char* str, struct _PropertyTable* prop, S2List* next) {
    S2List* sl = cons(str, next);
    sl->prop = prop;
    return sl;
}

PropertyTable* defaultPropertyTable() {
    PropertyTable* table = malloc(sizeof(PropertyTable));
    table->density = 0.0;
    table->fixed = 1;
    table->pressure = 1.0;
    table->argb = 0xFFFFFFFF;
    return table;
}

void destroyPropertyTable(PropertyTable* table) {
    S2List *node;
    node = table->reactions;
    while (node) {
	destroyList(node->lst);
	node = node->next;
    }
    destroyList(table->reactions);
    destroyList(table->decay);
    free(table);
}

unsigned char to8(float x) {
    if (x < 0.0) return 0;
    else if (x > 1.0) return 255;
    else return (unsigned char)(255.0 * x + 0.5);
}

unsigned int toARGB(float r, float g, float b) {
    return 0xFF000000 | (to8(r) << 16) | (to8(g) << 8) | to8(b);
}

Sand2Spec* makeSpec(S2List* properties, S2List* menu) {
    Sand2Spec* spec = malloc(sizeof(Sand2Spec));
    spec->properties = properties;
    spec->menu = menu;
    return spec;
}

void destroySpec(Sand2Spec* spec) { 
    S2List *node;
    node = spec->properties;
    while (node) {
	destroyPropertyTable(node->prop);
	node = node->next;
    }
    destroyList(spec->properties);
    destroyList(spec->menu);
    free(spec);
}
