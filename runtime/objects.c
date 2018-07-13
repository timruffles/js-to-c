#include "./lib/uthash.h"

#include "language.h"
#include "objects.h"
#include "exceptions.h"

typedef struct {
    JsValue *name;
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

JsValue* objectGet(JsValue *val, JsValue *name) {
    // TODO type assertion on val
    JsObject* object = jsValuePointer(val);
    PropertyDescriptor *descriptor;
    HASH_FIND_PTR(object->properties, &name, descriptor);
    return descriptor == NULL
           ? getUndefined()
           : descriptor->value;
}

PropertyDescriptor* propertyCreate() {
    PropertyDescriptor *pd = calloc(sizeof(PropertyDescriptor), 1);
    return pd;
}

JsValue* objectSet(JsValue* val, JsValue* name, JsValue* value) {
    // TODO type assertion on val
    JsObject* object = jsValuePointer(val);
    PropertyDescriptor *descriptor = propertyCreate();
    *descriptor = (PropertyDescriptor) {
        .name = name,
        .value = value,
    };
    PropertyDescriptor* replaced;
    HASH_REPLACE_PTR(object->properties, name, descriptor, replaced);
    return value;
}




void objectDestroy(JsValue *object) {
    free(object);
}


