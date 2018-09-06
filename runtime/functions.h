#include "environments.h"
#include "language.h"

// the C function that implements the function
typedef JsValue* (TargetFunction)(Env*);
typedef struct FunctionRecord FunctionRecord;

JsValue* functionCreate(TargetFunction* function, JsValue* argumentNames[], uint64_t argCount, Env*);
JsValue* _functionRun(JsValue* function, Env* env);
JsValue* functionRunWithArguments(JsValue* val, JsValue* argumentValues[], uint64_t argumentCount, JsValue* thisValue);
