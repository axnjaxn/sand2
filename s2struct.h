#ifndef _BPJ_SAND2_STRUCTS_H
#define _BPJ_SAND2_STRUCTS_H

struct _StringList {
    char str[256];
    struct _StringList* next;
};
typedef struct _StringList StringList;

struct _StringList* cons(const char* str, struct _StringList* next);
void printStringList(struct _StringList* sl);
void destroyStringList(struct _StringList* sl);

#endif
