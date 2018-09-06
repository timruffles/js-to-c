#include <stdlib.h>
#include <stdio.h>

#include "assert.h"
#include "environments.h"
#include "language.h"
#include "objects.h"
#include "runtime.h"
#include "functions.h"
#include "strings.h"
#include "gc.h"
#include "lib/debug.h"


// compiled function
typedef struct FunctionRecord {
    GcHeader;
    TargetFunction* function;
    JsValue** argumentNames;
    uint64_t argumentCount;
    Env* env;
} FunctionRecord;

JsValue* functionRunWithArguments(JsValue* val, JsValue* argumentValues[], uint64_t argumentCount, JsValue* thisValue) {
    log_info("Asserting function type");
    if(jsValueType(val) != FUNCTION_TYPE) {
      log_err("Expected function got %s\n", jsValueReflect(val).name);
      assert(false);
    }
    log_info("Getting fn record");
    FunctionRecord* record = objectGetCallInternal(val);
    log_info("Fn record %p", record);
    assert(record->argumentCount == argumentCount);
    Env* callEnv = envCreateForCall(record->env, record->argumentNames, argumentValues, record->argumentCount);

    // setup this - either passed from context, or unset, meaning global
    envDeclare(callEnv, stringFromLiteral("this"));
    envSet(callEnv, stringFromLiteral("this"), thisValue == NULL ? runtimeGet()->global : thisValue);

    log_info("Executing");
    return _functionRun(val, callEnv);
}

JsValue* _functionRun(JsValue* val, Env* env) {
    return objectGetCallInternal(val)->function(env);
}

JsValue* functionCreate(TargetFunction* function, JsValue* argumentNames[], uint64_t argCount, Env* env) {
    FunctionRecord* record = gcAllocate(sizeof(FunctionRecord), FUNCTION_RECORD_TYPE);
    record->function = function;
    record->argumentNames = argumentNames;
    record->argumentCount = argCount;
    record->env = env;
    JsValue* val = objectCreateFunction(record);
    return val;
}
