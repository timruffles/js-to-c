#include "./lib/uthash.h"

#include "language.h"
#include "objects.h"
#include "exceptions.h"
#include "strings.h"
#include "functions.h"
#include "gc.h"
#include "lib/debug.h"

#define OBJECT_VALUE(V) ((JsObject*)jsValuePointer(V))

typedef struct PropertyDescriptor PropertyDescriptor;

typedef struct PropertyDescriptor {
    GcHeader;

    char* name;
    JsValue *value;

    PropertyDescriptor* nextProperty;
} PropertyDescriptor;

typedef struct JsObject {
    GcHeader;

    PropertyDescriptor* properties;
    PropertyDescriptor* tailProperty;

    JsValue* prototype;

    // determines if this is callable - i.e [[Call]] internal slot
    FunctionRecord* callInternal;
} JsObject;

/**
 * A 'plain' object - with Object as prototype
 */
JsValue* objectCreatePlain() {
    // TODO set pt
    JsObject *obj = gcAllocate(sizeof(JsObject), OBJECT_VALUE_TYPE);
    JsValue *val = jsValueCreatePointer(OBJECT_TYPE, obj);
    return val;
}

JsValue* objectCreate(JsValue* prototype) {
    JsValue *obj = objectCreatePlain();
    OBJECT_VALUE(obj)->prototype = prototype;
    return obj;
}

JsValue* objectCreateFunction(FunctionRecord* fr) {
    // TODO set function prototype
    JsObject *obj = gcAllocate(sizeof(JsObject), OBJECT_VALUE_TYPE);
    obj->callInternal = fr;

    JsValue *val = jsValueCreatePointer(FUNCTION_TYPE, obj);
    return val;
}
 
// used from compiled code
JsValue* objectGet(JsValue *val, JsValue *name) {
    JsValue* found = objectLookup(val, name);
    return found == NULL
      ? getUndefined()
      : found;
}

FunctionRecord* objectGetCallInternal(JsValue *val) {
    return OBJECT_VALUE(val)->callInternal;
}

static PropertyDescriptor* findProperty(PropertyDescriptor *pd, char *name) {
    while(pd != NULL) {
        if(strcmp(pd->name, name) == 0) {
            return pd;
        }
        pd = pd->nextProperty;
    }
    return NULL;
}

JsValue* objectInternalOwnProperty(JsValue* value, JsValue* name) {
    char* cString = stringGetCString(name);
    PropertyDescriptor* pd = findProperty(OBJECT_VALUE(value)->properties
            , cString);
    return pd == NULL
        ? NULL
        : pd->value;
}


// https://www.ecma-international.org/ecma-262/5.1/#sec-9.9
bool* isCoercibleToObject(JsValue* val) {
    switch(jsValueType(val)) {
        case UNDEFINED_TYPE:
        case NULL_TYPE:
            return false;

        default:
            return true;
    }
}


// returns NULL or pointer to JsValue*
JsValue* objectLookup(JsValue *val, JsValue *name) {
    char* cString = stringGetCString(name);

    // starting with object, and going up prototype chain, find a matching
    // property
    JsValue* target = val;
    while(1) {
        JsObject* object = jsValuePointer(target);
        PropertyDescriptor* descriptor = findProperty(object->properties, cString);
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
    PropertyDescriptor *pd = gcAllocate(sizeof(PropertyDescriptor), PROPERTY_DESCRIPTOR_TYPE);
    return pd;
}

static void appendProperty(JsObject* object, PropertyDescriptor* pd) {
    if(object->properties == NULL) {
        object->properties = pd;
    } else {
        object->tailProperty->nextProperty = pd;
    }
    object->tailProperty = pd;
}

// used from compiled code
JsValue* objectSet(JsValue* objectVal, JsValue* name, JsValue* value) {
    // TODO type assertion on object
    JsObject* object = jsValuePointer(objectVal);
    char* nameString = stringGetCString(name);

    PropertyDescriptor *descriptor = findProperty(object->properties, nameString);
    if(descriptor == NULL) {
        descriptor = propertyCreate();
        descriptor->name = nameString;
        appendProperty(object, descriptor);
    }

    descriptor->value = value;

    return value;
}

/**
 * Get parent environment
 */
JsValue* objectEnvGetParent(JsValue* env) {
    return ((JsObject*)jsValuePointer(env))->prototype;
}

void objectGcTraverse(JsValue* value, GcCallback* cb) {
    JsObject* object = jsValuePointer(value);
    cb(object);

    if(object->prototype != NULL) {
        cb(object->prototype);
    }

    for(PropertyDescriptor* pd = object->properties;
        pd != NULL;
        pd = pd->nextProperty
    ) {
        cb(pd);
        cb(pd->value);
    }
}

void objectDestroy() {
    // NOOP
}

