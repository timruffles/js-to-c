
static Heap heapOne;
static Heap heapTwo;
static Heap* activeHeap = &heapOne;
static Heap* nextHeap = &heapTwo;

void gcInit() {
    heapInit(&heapOne, HEAP_SIZE());
    heapInit(&heapTwo, HEAP_SIZE());
}

void gcReset() {
    heapFree(&heapOne);
    heapFree(&heapTwo);
    gcInit();
}

//void envWalk(JsValue* value, void (callback)(JsValue*)) {
//    Env* env = (Env*) value->value.pointer;
//    for(uint64_t i = 0; i < env->size; i++) {
//        JsValue* found = env->values[i];
//        callback(found);
//        env->values[i] = found->movedTo;
//    }
//}
