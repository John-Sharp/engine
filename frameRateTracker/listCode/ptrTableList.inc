#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <stdio.h>

bool ptrTableCmp(const ptrTableNode * a, const ptrTableNode * b)
{
    return (memcmp(a, b, sizeof(*a)) == 0);
}

ptrTableList * ptrTableListAdd(ptrTableList * l, ptrTableNode * val)
{
    ptrTableList * nn = malloc(sizeof(*nn));
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

ptrTableList * ptrTableListRm(ptrTableList * l, const ptrTableNode * val, bool (*cmp)(const ptrTableNode * a, const ptrTableNode * b), ptrTableNode ** removedVal)
{
    ptrTableList * n;
    ptrTableList * nPrev = NULL;
    if (removedVal)
        *removedVal = NULL;
    for (n = l; n != NULL; n = n->next)
    {
        if (cmp(val, (n->val)))
        {
            if (!nPrev)
            {
                ptrTableList * newList = n->next;
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

ptrTableList * ptrTableListSearch(ptrTableList * l, const ptrTableNode * val, bool (*cmp)(const ptrTableNode * a, const ptrTableNode * b))
{
    ptrTableList * n;
    for (n = l; n != NULL; n = n->next)
    {
        if (cmp(val, (n->val)))
        {
            return n;
        }
    }

    return NULL;
}
