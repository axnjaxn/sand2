#include "s2struct.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct _StringList* cons(const char* str, struct _StringList* next) {
    struct _StringList* sl = malloc(sizeof(struct _StringList));
    strcpy(sl->str, str);
    sl->next = next;
    return sl;
}

void printStringList(struct _StringList* sl) {
    struct _StringList* next;
    while (sl) {
	next = sl->next;
	printf("%s\n", sl->str);
	sl = sl->next;
    }
}

void destroyStringList(struct _StringList* sl) {
    struct _StringList* next;
    while (sl) {
	next = sl->next;
	free(sl);
	sl = sl->next;
    }
}

