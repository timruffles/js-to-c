#include "gc.h"
#include "_memory.h"
#include "language.h"
#include "runtime.h"
#include "event.h"
#include "lib/debug.h"

static const uint64_t NO_REPEAT = 0;

typedef struct {
    JsValue* fn;
    FunctionArguments args;
} TimerCallback;

void eventLoop() {
    log_info("starting event loop");
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
    uv_loop_close(uv_default_loop());
}

static void timerRun(uv_timer_t* handle) {
    log_info("timer run");
    TimerCallback* tcb = handle->data;
    runtimeRunCallback(tcb->fn, tcb->args);
}

void eventTimeout(JsValue* fn, FunctionArguments args, uint64_t ms) {
    uv_timer_t* handle;
    ensureCallocBytes(handle, sizeof(uv_timer_t));
    uv_timer_init(uv_default_loop(), handle);

    TimerCallback* callback;
    ensureCallocBytes(callback, sizeof(TimerCallback));
    *callback = (TimerCallback){
      .fn = fn,
      .args = args,  
    };
    handle->data = (void*)callback;

    uv_timer_start(handle, &timerRun, ms, NO_REPEAT);
}
