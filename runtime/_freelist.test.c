#pragma clang diagnostic ignored "-Wmissing-prototypes"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "test.h"
#include "gc.h"
#include "_freelist.h"
#include "_memory.h"
#include "config.h"
#include "runtime.h"

static char* printIntList(FreeNode* list) {
    char* output;
    ensureCallocBytes(output, 3200);
    FREE_LIST_ITERATE(&list, node) {
        char buf[16] = {0};
        sprintf(buf, "%i ", *((int*)node->value));
        strcat(output, buf);
    }
    return output;
}

static void itCanCreateAList() {
    FreeNode* list = NULL;
    log_info("%p %p", list, &list);
    int value = 5;
    FreeNode* node = freeListAppend(&list, &value);
    assert(list != NULL);
    assert(*(int*)(node->value) == 5);
}

static void itAppends() {
    FreeNode* list = NULL;
    int valueA = 5;
    int valueB = 10;
    freeListAppend(&list, &valueA);
    FreeNode* node = freeListAppend(&list, &valueB);
    assert(*(int*)(node->value) == 10);
}

static void itCanDelete() {
    FreeNode* list = NULL;
    int valueA = 1;
    int valueB = 20;
    int valueC = 300;
    FreeNode* nodeA = freeListAppend(&list, &valueA);
    FreeNode* nodeB = freeListAppend(&list, &valueB);
    FreeNode* nodeC = freeListAppend(&list, &valueC);

    // delete middle node of C <-> B <-> A
    freeListDelete(&list, nodeB);

    // assert C <-> A
    assertPointersEqual(nodeC->next, nodeA);
    assertPointersEqual(nodeA->prev, nodeC);

    assertStringEqual("300 1 ", printIntList(list));

}

static void itAppendsAfterDelete() {
    FreeNode* list = NULL;
    int valueA = 5;
    int valueB = 10;
    FreeNode* nodeA = freeListAppend(&list, &valueA);
    freeListDelete(&list, nodeA);
    FreeNode* nodeB = freeListAppend(&list, &valueB);
    assert(*(int*)(nodeB->value) == 10);
}


static void itClears() {
    FreeNode* list = NULL;
    int valueA = 5;
    freeListAppend(&list, &valueA);
    freeListClear(&list);
}

static void itWorksAfterClear() {
    FreeNode* list = NULL;
    int valueA = 5;
    int valueB = 10;
    freeListAppend(&list, &valueA);
    freeListClear(&list);

    FreeNode* nodeB = freeListAppend(&list, &valueB);
    assert(*(int*)(nodeB->value) == 10);
}

static void itIterates() {
    char* expected = "10 5 ";
    FreeNode* list = NULL;
    int valueA = 5;
    int valueB = 10;
    freeListAppend(&list, &valueA);
    freeListAppend(&list, &valueB);

    assertStringEqual(expected, printIntList(list));
}

static void itWorksAfterASeriesOfOperations() {
    FreeNode* list = NULL;
    int valueOne = 1;
    int valueTwo = 2;
    int valueThree = 3;
    int valueFour = 4;

    // add and delete a single node
    FreeNode* oneA = freeListAppend(&list, &valueOne);
    freeListDelete(&list, oneA);
    assertStringEqual("", printIntList(list));

    // add two nodes, delete head, add a third
    FreeNode* oneB = freeListAppend(&list, &valueOne);
    FreeNode* two = freeListAppend(&list, &valueTwo);
    freeListDelete(&list, oneB);
    FreeNode* three = freeListAppend(&list, &valueThree);
    assertStringEqual("3 2 ", printIntList(list));

    // delete tail, add node 
    freeListDelete(&list, two);
    FreeNode* four = freeListAppend(&list, &valueFour);
    assertStringEqual("4 3 ", printIntList(list));
}

int main() {
    test(itCanCreateAList);
    test(itAppends);
    test(itAppendsAfterDelete);
    test(itCanDelete);
    test(itClears);
    test(itWorksAfterClear);
    test(itIterates);
    test(itWorksAfterASeriesOfOperations);
}
