#include <stdlib.h>

#include "_freelist.h"
#include "_memory.h"
#include "lib/debug.h"

/**
 * Simple LL library - ensure you create new lists by assigning them to NULL.
 */

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
    if(*list == NULL) {
        *list = toAdd;
    } else {
        FreeNode* currentHead = *list;
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
