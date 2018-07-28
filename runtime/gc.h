
void gcRun(void);
void moveValue(JsValue*);

void gcRun() {
    printf("GC run\n");

    moveValue(globalEnv);
    globalEnv = globalEnv->movedTo;

    envWalk(globalEnv, moveValue);

    Heap* oldActive = activeHeap;
    activeHeap = nextHeap;
    nextHeap = oldActive;

    heapEmpty(oldActive);
}
