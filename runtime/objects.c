#include "./lib/uthash.h"

#include "language.h"
#include "objects.h"
#include "exceptions.h"

typedef struct {
    JsValue *property;
    JsValue *value;

    UT_hash_handle hh;
} PropertyDescriptor;

typedef struct JsObject {
    PropertyDescriptor* properties;
    JsObject* prototype;
} JsObject;

/**
 * A 'plain' object - with Object as prototype
 */
JsValue* objectCreatePlain() {
    JsObject *obj = calloc(sizeof(JsObject), 1);
    JsValue *val = jsValueCreatePointer(OBJECT_TYPE, obj);
    return val;
}

JsValue *objectGet(JsValue *val, JsValue *name) {
    JsObject* object = jsValuePointer(val);
    PropertyDescriptor *descriptor;
    HASH_FIND_PTR(object->properties, &name, descriptor);
    return descriptor == NULL
           ? getUndefined()
           : descriptor->value;
}

JsValue * objectSet(JsValue* val, JsValue* name, JsValue* value) {
    JsObject* object = jsValuePointer(val);
    PropertyDescriptor *descriptor;
    HASH_FIND_PTR(object->properties, &name, descriptor);
    if (descriptor == NULL) {
        throwError("Attempted to set undeclared variable");
    }
    descriptor->value = value;
    return value;
}

void objectDestroy(JsValue *object) {
    free(object);
}


