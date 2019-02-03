#pragma once
#define FREE_LIST_ITERATE(L, V) for(FreeNode* V = *L; V != NULL; V = V->next)

// space tracking, which lives outside JS heap
typedef struct FreeNode {
    struct FreeNode* next;
    struct FreeNode* prev;
    void* value;
} FreeNode;

void freeListCreate(FreeNode** list, void* value);
void freeListDelete(FreeNode** list, FreeNode* toRemove);
FreeNode* freeListAppend(FreeNode** list, void* toAdd);
void freeListClear(FreeNode** list);
