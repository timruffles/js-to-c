typedef struct StackFrame {
    Env* env;
    struct StackFrame* parent;
} StackFrame;

static StackFrame* activeFrame;

void stackWalk(StackFrame* frame, void (callback)(JsValue*)) {
    envWalk(frame->env, callback);
    if(frame->parent) {
        stackWalk(frame->parent, callback);
    }
}
