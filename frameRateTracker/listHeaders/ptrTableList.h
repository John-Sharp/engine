#include <stdbool.h>

// typedef struct ptrTableList ptrTableList;
typedef struct ptrTableList ptrTableList;
struct ptrTableList {
    ptrTableNode * val;
    ptrTableList * next;    
};

bool ptrTableCmp(const ptrTableNode * a, const ptrTableNode * b);

// ptrTableListAdd: adds element to head of list
ptrTableList * ptrTableListAdd(ptrTableList * l, ptrTableNode * val);

// ptrTableListRmremove: removes element from list
ptrTableList * ptrTableListRm(ptrTableList * l, const ptrTableNode * val, bool (*cmp)(const ptrTableNode * a, const ptrTableNode * b), ptrTableNode ** removedVal);

ptrTableList * ptrTableListSearch(ptrTableList * l, const ptrTableNode * val, bool (*cmp)(const ptrTableNode * a, const ptrTableNode * b));
