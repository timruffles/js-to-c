#include <stdlib.h>
#include <stdio.h>

#include "assert.h"
#include "environments.h"
#include "language.h"
#include "objects.h"
#include "functions.h"
#include "strings.h"
#include "gc.h"
#include "lib/debug.h"


// compiled function
typedef struct FunctionRecord {
    TargetFunction* function;
    JsValue** argumentNames;
    uint64_t argumentCount;
    Env* env;
} FunctionRecord;

JsValue* functionRunWithArguments(JsValue* val, JsValue* argumentValues[], uint64_t argumentCount) {
    log_info("Asserting function type");
    if(jsValueType(val) != FUNCTION_TYPE) {
      printf("Expected function got %s\n", jsValueReflect(val).name);
      assert(false);
    }
    log_info("Getting fn record");
    FunctionRecord* record = objectGetCallInternal(val);
    log_info("Fn record %p", record);
    assert(record->argumentCount == argumentCount);
    Env* callEnv = envCreateForCall(record->env, record->argumentNames, argumentValues, record->argumentCount);
    log_info("Executing");
    return _functionRun(val, callEnv);
}

JsValue* _functionRun(JsValue* val, Env* env) {
    return objectGetCallInternal(val)->function(env);
}

JsValue* functionCreate(TargetFunction* function, JsValue* argumentNames[], uint64_t argCount, Env* env) {
    FunctionRecord* record = gcAllocate2(sizeof(FunctionRecord), FUNCTION_TYPE);
    record->function = function;
    record->argumentNames = argumentNames;
    record->argumentCount = argCount;
    record->env = env;
    return objectCreateFunction(record);
}
