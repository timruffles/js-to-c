#include <stdlib.h>

#include "_freelist.h"
#include "_memory.h"

static FreeNode* freeNodeCreate(void* value) {
    FreeNode* node;
    ensureCallocBytes(node, sizeof(FreeNode));
    *node = (FreeNode) {
        .value = value,
    };
    return node;
}

void freeListDelete(FreeNode** list, FreeNode* toRemove) {
    // not first item
    if(toRemove == *list) {
        *list = toRemove->next;
    } else {
       toRemove->prev->next = toRemove->next;
       free(toRemove);
    }
}

FreeNode* freeListAppend(FreeNode** list, void* value) {
    FreeNode* toAdd = freeNodeCreate(value);
    // free list invariants:
    //   only head lacks a prev
    //   every other node has prev
    //   tail lacks a next
    FreeNode* currentHead = *list;
    if(currentHead == NULL) {
        *list = toAdd;
    } else {
        toAdd->next = currentHead;
        currentHead->prev = toAdd;
        *list = toAdd;
    }
    return toAdd;
}

void freeListClear(FreeNode** list) {
    FREE_LIST_ITERATE(list, node) {
        free(node);
    }
}
