#include <stdlib.h>
#include <stdio.h>

#include "assert.h"
#include "environments.h"
#include "language.h"
#include "objects.h"
#include "functions.h"
#include "strings.h"
#include "lib/debug.h"


typedef struct FunctionRecord {
    TargetFunction* function;
    JsValue** argumentNames;
    uint64_t argumentCount;
} FunctionRecord;

JsValue* functionRunWithArguments(JsValue* val, Env* parentEnv, JsValue* argumentValues[], uint64_t argumentCount) {
    log_info("Asserting function type");
    if(jsValueType(val) != FUNCTION_TYPE) {
      printf("Expected function got %s\n", jsValueType(val));
      assert(false);
    }
    log_info("Getting fn record");
    FunctionRecord* record = objectGetCallInternal(val);
    log_info("Fn record %p", record);
    assert(record->argumentCount == argumentCount);
    Env* callEnv = envCreateForCall(parentEnv, record->argumentNames, argumentValues, record->argumentCount);
    log_info("Executing");
    return functionRun(val, callEnv);
}

JsValue* functionRun(JsValue* val, Env* env) {
    return objectGetCallInternal(val)->function(env);
}

JsValue* functionCreate(TargetFunction* function, JsValue* argumentNames[], uint64_t argCount) {
    FunctionRecord* record = calloc(sizeof(FunctionRecord), 1);
    record->function = function;
    record->argumentNames = argumentNames;
    record->argumentCount = argCount;
    return objectCreateFunction(record);
}


//JsValue** functionGetArgumentNames(JsValue* val) {
//    assert(jsValueType(val) == FUNCTION_TYPE);
//    return objectGetCallInternal(val)->argumentNames;
//}
//
//uint64_t functionGetArgumentCount(JsValue* val) {
//    assert(jsValueType(val) == FUNCTION_TYPE);
//    return objectGetCallInternal(val)->argumentCount;
//}
