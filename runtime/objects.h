#pragma once

#include "language.h"
#include "functions.h"
#include "gc.h"

#define JS_SET_LITERAL(O,S,V) objectSet(O, stringFromLiteral(S), V)
#define JS_GET_LITERAL(O,S) objectGet(O, stringFromLiteral(S))
#define JS_SET(O,P,V) { GcAtomicId gid = gcAtomicGroupStart(); objectSet(O,stringFromLiteral(P),V); gcAtomicGroupEnd(gid); }
#define JS_GET(O,P) objectGet(O,stringFromLiteral(P))


typedef struct JsObject JsObject;

JsValue* objectCreate(JsValue* prototype);
JsValue* objectCreatePlain(void);
JsValue* objectCreateFunction(FunctionRecord*);

JsValue* objectGet(JsValue* object, JsValue* property);
JsValue* objectSet(JsValue* object, JsValue* property, JsValue* value);
void objectDestroy(JsValue* object);

// used internally by environments to model envs as objects
JsValue* objectLookup(JsValue* object, JsValue* property);
JsValue* objectInternalOwnProperty(JsValue* value, JsValue* property);
JsValue* objectEnvGetParent(JsValue* env);
char* objectDebug(JsValue* env);

JsValue* objectNewOperation(JsValue* function, JsValue* argumentValues[], uint64_t argumentCount);

// internal slot lookup
FunctionRecord* objectGetCallInternal(JsValue *);

void objectGcTraverse(JsValue* value, GcCallback* cb);

typedef struct ForOwnIterator {
    JsValue* property;
    const bool done;
    void* next;
} ForOwnIterator;
ForOwnIterator objectForOwnPropertiesIterator(JsValue* value);
ForOwnIterator objectForOwnPropertiesNext(ForOwnIterator iterator);


