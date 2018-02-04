#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <stdio.h>

bool preLogicCallBackCmp(const preLogicCallBack * a, const preLogicCallBack * b)
{
    return (memcmp(a, b, sizeof(*a)) == 0);
}

preLogicCallBackList * preLogicCallBackListAdd(preLogicCallBackList * l, preLogicCallBack * val)
{
    preLogicCallBackList * nn = malloc(sizeof(*nn));
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

preLogicCallBackList * preLogicCallBackListRm(preLogicCallBackList * l, const preLogicCallBack * val, bool (*cmp)(const preLogicCallBack * a, const preLogicCallBack * b), preLogicCallBack ** removedVal)
{
    preLogicCallBackList * n;
    preLogicCallBackList * nPrev = NULL;
    if (removedVal)
        *removedVal = NULL;
    for (n = l; n != NULL; n = n->next)
    {
        if (cmp(val, (n->val)))
        {
            if (!nPrev)
            {
                preLogicCallBackList * newList = n->next;
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

preLogicCallBackList * preLogicCallBackListSearch(preLogicCallBackList * l, const preLogicCallBack * val, bool (*cmp)(const preLogicCallBack * a, const preLogicCallBack * b))
{
    preLogicCallBackList * n;
    for (n = l; n != NULL; n = n->next)
    {
        if (cmp(val, (n->val)))
        {
            return n;
        }
    }

    return NULL;
}
