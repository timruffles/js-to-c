#include <stdlib.h>

#include "environments.h"
#include "language.h"
#include "objects.h"
#include "functions.h"
#include "strings.h"


typedef struct FunctionRecord {
    TargetFunction* function;
} FunctionRecord;


JsValue* functionRun(JsValue* val, Env* env) {
    return objectGetCallInternal(val)->function(env);
}

FunctionRecord* functionRecordCreate(TargetFunction* function) {
    FunctionRecord* record = calloc(sizeof(FunctionRecord), 1);
    record->function = function;
    return record;
}

JsValue* functionCreate(FunctionRecord* function) {
    return objectCreateFunction(function);
}


