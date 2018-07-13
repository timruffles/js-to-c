#include "./lib/uthash.h"

#include "language.h"
#include "objects.h"
#include "exceptions.h"
#include "strings.h"

typedef struct {
    char* name;
    JsValue *value;

    UT_hash_handle hh;
} PropertyDescriptor;

typedef struct JsObject {
    PropertyDescriptor* properties;
    JsValue* prototype;
} JsObject;

/**
 * A 'plain' object - with Object as prototype
 */
JsValue* objectCreatePlain() {
    JsObject *obj = calloc(sizeof(JsObject), 1);
    JsValue *val = jsValueCreatePointer(OBJECT_TYPE, obj);
    return val;
}

JsValue* objectCreate(JsValue* prototype) {
    JsValue *obj = objectCreatePlain();
    ((JsObject*)jsValuePointer(obj))->prototype = prototype;
    return obj;
}

JsValue* objectGet(JsValue *val, JsValue *name) {
    // TODO type assertion on val
    JsObject* object = jsValuePointer(val);
    PropertyDescriptor *descriptor;

    JsString* nameString = stringGet(name);
    HASH_FIND_STR(object->properties, nameString->cString, descriptor);
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

    JsString* nameString = stringGet(name);
    PropertyDescriptor *descriptor;
    HASH_FIND_STR(object->properties, nameString->cString, descriptor);

    if(descriptor == NULL) {
        descriptor = propertyCreate();
        JsString* nameString = stringGet(name);
        descriptor->name = nameString->cString;
        HASH_ADD_KEYPTR(
          hh,
          object->properties,
          nameString->cString,
          nameString->length,
          descriptor
        );
    }

    descriptor->value = value;

    return value;
}

void objectDestroy(JsValue *object) {
    free(object);
}


