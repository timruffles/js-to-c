#include "./lib/uthash.h"

#include "language.h"
#include "objects.h"
#include "exceptions.h"
#include "strings.h"
#include "functions.h"
#include "lib/debug.h"

typedef struct {
    char* name;
    JsValue *value;

    UT_hash_handle hh;
} PropertyDescriptor;

typedef struct JsObject {
    PropertyDescriptor* properties;
    JsValue* prototype;

    // can we call this - kinda like [[Call]]?
    FunctionRecord* callInternal;
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

JsValue* objectCreateFunction(FunctionRecord* fr) {
    // TODO set function prototype
    JsObject *obj = calloc(sizeof(JsObject), 1);
    obj->callInternal = fr;

    JsValue *val = jsValueCreatePointer(FUNCTION_TYPE, obj);
    return val;
}

JsValue* objectGet(JsValue *val, JsValue *name) {
    log_info("Object lookup %s", stringGetCString(name));
    JsValue* found = objectLookup(val, name);
    return found == NULL
      ? getUndefined()
      : found;
}

FunctionRecord* objectGetCallInternal(JsValue *val) {
    return ((JsObject*)jsValuePointer(val))->callInternal;
}

JsValue* objectLookup(JsValue *val, JsValue *name) {
    // TODO type assertion on val
    char* cString = stringGetCString(name);
    PropertyDescriptor *descriptor;

    // starting with object, and going up prototype chain, find a matching
    // property
    JsValue* target = val;
    while(1) {
        JsObject* object = jsValuePointer(target);
        HASH_FIND_STR(object->properties, cString, descriptor);
        if(descriptor != NULL) {
            return descriptor->value;
        }

        // go up pt chain one step, or return NULL if we're out of pts
        target = ((JsObject*)jsValuePointer(target))->prototype;
        if(target == NULL) {
            return NULL;
        }
    }
}

static PropertyDescriptor* propertyCreate() {
    PropertyDescriptor *pd = calloc(sizeof(PropertyDescriptor), 1);
    return pd;
}

JsValue* objectSet(JsValue* objectVal, JsValue* name, JsValue* value) {
    // TODO type assertion on object
    JsObject* object = jsValuePointer(objectVal);
    char* nameString = stringGetCString(name);

    PropertyDescriptor *descriptor;
    HASH_FIND_STR(object->properties, nameString, descriptor);

    if(descriptor == NULL) {
        descriptor = propertyCreate();
        descriptor->name = nameString;
        HASH_ADD_KEYPTR(
          hh,
          object->properties,
          nameString,
          stringLength(name),
          descriptor
        );
    }

    descriptor->value = value;

    return value;
}

void objectDestroy(JsValue *object) {
    free(object);
}


