#include <stdbool.h>

// typedef struct preLogicCallBackList preLogicCallBackList;
typedef struct preLogicCallBackList preLogicCallBackList;
struct preLogicCallBackList {
    preLogicCallBack * val;
    preLogicCallBackList * next;    
};

bool preLogicCallBackCmp(const preLogicCallBack * a, const preLogicCallBack * b);

// preLogicCallBackListAdd: adds element to head of list
preLogicCallBackList * preLogicCallBackListAdd(preLogicCallBackList * l, preLogicCallBack * val);

// preLogicCallBackListRmremove: removes element from list
preLogicCallBackList * preLogicCallBackListRm(preLogicCallBackList * l, const preLogicCallBack * val, bool (*cmp)(const preLogicCallBack * a, const preLogicCallBack * b), preLogicCallBack ** removedVal);

preLogicCallBackList * preLogicCallBackListSearch(preLogicCallBackList * l, const preLogicCallBack * val, bool (*cmp)(const preLogicCallBack * a, const preLogicCallBack * b));
