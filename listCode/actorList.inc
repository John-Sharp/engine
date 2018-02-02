#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <stdio.h>

bool actorCmp(const actor * a, const actor * b)
{
    return (memcmp(a, b, sizeof(*a)) == 0);
}

actorList * actorListAdd(actorList * l, actor * val)
{
    actorList * nn = malloc(sizeof(*nn));
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

actorList * actorListRm(actorList * l, const actor * val, bool (*cmp)(const actor * a, const actor * b), actor ** removedVal)
{
    actorList * n;
    actorList * nPrev = NULL;
    if (removedVal)
        *removedVal = NULL;
    for (n = l; n != NULL; n = n->next)
    {
        if (cmp(val, (n->val)))
        {
            if (!nPrev)
            {
                actorList * newList = n->next;
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

actorList * actorListSearch(actorList * l, const actor * val, bool (*cmp)(const actor * a, const actor * b))
{
    actorList * n;
    for (n = l; n != NULL; n = n->next)
    {
        if (cmp(val, (n->val)))
        {
            return n;
        }
    }

    return NULL;
}
