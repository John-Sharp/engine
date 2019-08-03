#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <stdio.h>

bool ptrTableListCmp(const ptrTableNode * a, const ptrTableNode * b)
{
    return (memcmp(a, b, sizeof(*a)) == 0);
}

ptrTableListList * ptrTableListListAdd(ptrTableListList * l, ptrTableNode * val)
{
    ptrTableListList * nn = malloc(sizeof(*nn));
    if (!nn)
    {
        fprintf(stderr, "error allocating memory for list\n");
        exit(1);
        return l;
    }
    nn->val = val;
    nn->next = l;

    return nn;
}

ptrTableListList * ptrTableListListRm(ptrTableListList * l, const ptrTableNode * val, bool (*cmp)(const ptrTableNode * a, const ptrTableNode * b), ptrTableNode ** removedVal)
{
    ptrTableListList * n;
    ptrTableListList * nPrev = NULL;
    if (removedVal)
        *removedVal = NULL;
    for (n = l; n != NULL; n = n->next)
    {
        if (cmp(val, (n->val)))
        {
            if (!nPrev)
            {
                ptrTableListList * newList = n->next;
                if (removedVal)
                    *removedVal = n->val;
                free(n);
                return newList;
            }
            nPrev->next = n->next;
            if (removedVal)
                *removedVal = n->val;
            free(n);
            return l;
        }
    }

    return l;
}

ptrTableListList * ptrTableListListSearch(ptrTableListList * l, const ptrTableNode * val, bool (*cmp)(const ptrTableNode * a, const ptrTableNode * b))
{
    ptrTableListList * n;
    for (n = l; n != NULL; n = n->next)
    {
        if (cmp(val, (n->val)))
        {
            return n;
        }
    }

    return NULL;
}
