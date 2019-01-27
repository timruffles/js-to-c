#pragma clang diagnostic ignored "-Wmissing-prototypes"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "test.h"
#include "gc.h"
#include "_freelist.h"
#include "config.h"
#include "runtime.h"

static void itCanCreateAList() {
    FreeNode* list;
    int value = 5;
    FreeNode* node = freeListAppend(&list, &value);
    assert(*(int*)(node->value) == 5);
}

static void itAppends() {
    FreeNode* list;
    int valueA = 5;
    int valueB = 10;
    freeListAppend(&list, &valueA);
    FreeNode* node = freeListAppend(&list, &valueB);
    assert(*(int*)(node->value) == 10);
}

static void itCanDelete() {
    FreeNode* list;
    int valueA = 5;
    FreeNode* nodeA = freeListAppend(&list, &valueA);
    freeListDelete(&list, nodeA);
}

static void itAppendsAfterDelete() {
    FreeNode* list;
    int valueA = 5;
    int valueB = 10;
    FreeNode* nodeA = freeListAppend(&list, &valueA);
    freeListDelete(&list, nodeA);
    FreeNode* nodeB = freeListAppend(&list, &valueB);
    assert(*(int*)(nodeB->value) == 10);
}


static void itClears() {
    FreeNode* list;
    int valueA = 5;
    freeListAppend(&list, &valueA);
    freeListClear(&list);
}

static void itWorksAfterClear() {
    FreeNode* list;
    int valueA = 5;
    int valueB = 10;
    freeListAppend(&list, &valueA);
    freeListClear(&list);

    FreeNode* nodeB = freeListAppend(&list, &valueB);
    assert(*(int*)(nodeB->value) == 10);
}

static void itIterates() {
    char* expected = "5 10 ";
    char output[100];
    FreeNode* list;
    int valueA = 5;
    int valueB = 10;
    freeListAppend(&list, &valueA);
    freeListAppend(&list, &valueB);

    log_info("pre it");
    FREE_LIST_ITERATE(&list, node) {
        log_info("it to %p %p", node, node->value);
        char buf[16];
        sprintf(buf, "%i ", *((int*)node->value));
        log_info("sprinted");
        strcat(output, buf);
    }

    log_info("%s %s", expected, output);
    assert(strcmp(expected, output) == 0);
}

int main() {
    test(itCanCreateAList);
    test(itAppends);
    test(itAppendsAfterDelete);
    test(itCanDelete);
    test(itClears);
    test(itWorksAfterClear);
    test(itIterates);
}
