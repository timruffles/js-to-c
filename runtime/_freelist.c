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

void freeListDelete(FreeNode** list, FreeNode* toRemove) {
    // first item
    if(toRemove == *list) {
        FreeNode* next = *list = toRemove->next;
        if(next != NULL) {
            next->prev = NULL;
        }
    } else if (toRemove->prev->next == toRemove) {
       toRemove->prev->next = toRemove->next;
       FreeNode* next = toRemove->next;
       if(next != NULL) {
           next->prev = toRemove->prev;
       }
       free(toRemove);
    } else {
        // not in list
        fail("WARN: node to delete not in list %p (n: %p, p: %p)", toRemove, toRemove->next, toRemove->prev);
    }
}

void freeListClear(FreeNode** list) {
    FREE_LIST_ITERATE(list, node) {
        free(node);
    }
}
