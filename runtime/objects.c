#include "./lib/uthash.h"

#include "language.h"
#include "objects.h"
#include "exceptions.h"
#include "strings.h"
#include "functions.h"
#include "gc.h"
#include "lib/debug.h"

#define JS_VALUE_OBJECT_CREATE() jsValueCreatePointer(OBJECT_TYPE, sizeof(JsObject))

typedef struct {
    char* key;
    JsValue *name;
    JsValue *value;

    UT_hash_handle hh;
} PropertyDescriptor;

typedef struct JsObject {
    PropertyDescriptor* properties;
    JsValue* prototype;

    // determines if this is callable - i.e [[Call]] internal slot
    FunctionRecord* callInternal;
} JsObject;

/**
 * A 'plain' object - with Object as prototype
 */
JsValue* objectCreatePlain() {
    JsPointerAllocation alloc = JS_VALUE_OBJECT_CREATE();
    // TODO set pt
    return alloc.value;
}

JsValue* objectCreate(JsValue* prototype) {
    JsPointerAllocation alloc = JS_VALUE_OBJECT_CREATE();
    JsObject* object = alloc.pointer;
    object->prototype = prototype;
    return alloc.value;
}

JsValue* objectCreateFunction(FunctionRecord* fr) {
    JsPointerAllocation alloc = JS_VALUE_OBJECT_CREATE();
    JsObject* object = alloc.pointer;
    object->callInternal = fr;
    return alloc.value;
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
        descriptor->key = nameString;
        descriptor->name = name;
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

//GcState jsValueGcState(JsValue* value) {
//    return value->gcState;
//}

// walk an object tree, calling cb with every JsValue found
//JsValue* objectGc(JsValue* object, GcCallback* cb) {
//    JsValue* moved = cb(object);
//    object->movedTo = moved;
//
//    if(object->prototype) {
//        moved->prototype = objectGc(object->prototype);
//    }
//
//    moved->properties = NULL;
//    for(PropertyDescriptor* pd = object->properties;
//        pd != NULL;
//        pd = pd->hh.next
//    ) {
//        JsValue* valueMoved = jsValueGc(pd->value, cb);
//        objectSet(moved, pd->name, valueMoved);
//    }
//
//    return moved;
//}

void objectDestroy(JsValue *object) {
    // NOOP
}


