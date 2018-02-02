#include <stdbool.h>

// typedef struct actorList actorList;
typedef struct actorList actorList;
struct actorList {
    actor * val;
    actorList * next;    
};

bool actorCmp(const actor * a, const actor * b);

// actorListAdd: adds element to head of list
actorList * actorListAdd(actorList * l, actor * val);

// actorListRmremove: removes element from list
actorList * actorListRm(actorList * l, const actor * val, bool (*cmp)(const actor * a, const actor * b), actor ** removedVal);

actorList * actorListSearch(actorList * l, const actor * val, bool (*cmp)(const actor * a, const actor * b));
