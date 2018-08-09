#include "./lib/uthash.h"

#include "language.h"
#include "objects.h"
#include "exceptions.h"
#include "strings.h"
#include "functions.h"
#include "gc.h"
#include "lib/debug.h"


typedef struct {
    GcHeader;

    char* name;
    JsValue *value;

    UT_hash_handle hh;
} PropertyDescriptor;

typedef struct JsObject {
    GcHeader;

    PropertyDescriptor* properties;
    JsValue* prototype;

    // determines if this is callable - i.e [[Call]] internal slot
    FunctionRecord* callInternal;
} JsObject;

/**
 * A 'plain' object - with Object as prototype
 */
JsValue* objectCreatePlain() {
    JsObject *obj = gcAllocate(sizeof(JsObject));
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
    JsObject *obj = gcAllocate(sizeof(JsObject));
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
    PropertyDescriptor *pd = gcAllocate(sizeof(PropertyDescriptor));
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

//// walk an object tree, calling cb with every JsValue found
//JsValue* objectTraverseForGc(JsValue* object, ForOwnCallback* cb) {
//    JsValue* ptr = cb(object);
//    if(object->prototype) {
//        object->prototype = cb(object->prototype);
//    }
//
//    for(PropertyDescriptor* pd = object->properties;
//        pd != NULL;
//        pd = pd->hh.next) {
//        JsValueType type = jsValueType(pd->value);
//        if(type == OBJECT_TYPE || type == FUNCTION_TYPE) {
//            pd->value = objectTraverseDeep(pd->value, cb);
//        } else {
//            pd->value = cb(pd->property, pd->value);
//        }
//    }
//
//    return ptr;
//}

void objectDestroy(JsValue *object) {
    // NOOP
}


