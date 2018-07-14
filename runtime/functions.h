#include "environments.h"
#include "language.h"

// the C function that implements the function
typedef JsValue* (TargetFunction)(Env*);
typedef struct FunctionRecord FunctionRecord;

JsValue* functionCreate(FunctionRecord*);
FunctionRecord* functionRecordCreate(TargetFunction*);
JsValue* functionRun(JsValue* function, Env* env);
