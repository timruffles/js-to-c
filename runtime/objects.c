#include "./lib/uthash.h"

#include "language.h"
#include "objects.h"

typedef struct {
    JsValue *property;
    JsValue *value;

    UT_hash_handle hh;
} PropertyDescriptor;

typedef struct JsObject {
    PropertyDescriptor* descriptors;
    JsObject* prototype;
} JsObject;

/**
 * A 'plain' object - with Object as prototype
 */
JsValue* objectCreatePlain() {
    JsObject *obj = calloc(sizeof(JsObject), 1);
    JsValue *val = jsValueCreate(OBJECT_TYPE, obj);
    return val;
}

//JsValue *objectGet(JsValue *object, JsValue *name) {
//    PropertyDescriptor *descriptor;
//    HASH_FIND_PTR(object->properties, &name, descriptor);
//    return descriptor == NULL
//           ? getUndefined()
//           : descriptor->value;
//}
//
//void objectSet(JsValue *object, JsValue *name, JsValue *value) {
//    PropertyDescriptor *descriptor;
//    HASH_FIND_PTR(object->properties, &name, descriptor);
//    if (descriptor == NULL) {
//        throwError("Attempted to set undeclared variable");
//    }
//    descriptor->value = value;
//}

void objectDestroy(JsValue *object) {
    free(object);
}


