#ifndef _BPJ_SAND2_STRUCTS_H
#define _BPJ_SAND2_STRUCTS_H

struct _S2List {
    char str[256];
    union {
	float p;
	struct _S2List* lst;
	struct _PropertyTable* prop;
    };
    struct _S2List* next;
};
typedef struct _S2List S2List;

S2List* cons(const char* str, S2List* next);
void destroyList(S2List* sl);

S2List* addProb(const char* str, float p, S2List* next);
S2List* addList(const char* str, S2List* lst, S2List* next);
S2List* addProperties(const char* str, struct _PropertyTable* prop, S2List* next);

struct _PropertyTable {
    float density;
    int fixed;
    unsigned int argb;
    struct _S2List* reactions; //List of lists
    struct _S2List* decay; //List of probabilities
};
typedef struct _PropertyTable PropertyTable;

PropertyTable* defaultPropertyTable();
void destroyPropertyTable(PropertyTable* table);
unsigned int toARGB(float r, float g, float b);

struct _Sand2Spec {
    struct _S2List* names;
    struct _S2List* properties; //List of Property Tables
};
typedef struct _Sand2Spec Sand2Spec;

Sand2Spec* makeSpec(S2List* names, S2List* properties);
void destroySpec(Sand2Spec* spec);

#endif
