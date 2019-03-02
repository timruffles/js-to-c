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

    JsValue *name;
    JsValue *value;

    PropertyDescriptor* nextProperty;
} PropertyDescriptor;

typedef struct JsObject {
    GcHeader;

    PropertyDescriptor* properties;
    PropertyDescriptor* tailProperty;

    // Internal slots - implementation visible only
    JsValue* prototype;

    // determines if this is callable - i.e [[Call]] internal slot
    FunctionRecord* callInternal;
} JsObject;

/**
 * A 'plain' object - with Object as prototype
 */
JsValue* objectCreatePlain() {
    // TODO set pt
    JsObject *obj;
    JsValue *val;
    jsValueCreatePointer(val, OBJECT_TYPE, obj, OBJECT_VALUE_TYPE, sizeof(JsObject));
    return val;
}

JsValue* objectCreate(JsValue* prototype) {
    JsValue *obj = objectCreatePlain();
    OBJECT_VALUE(obj)->prototype = prototype;
    return obj;
}

JsValue* objectCreateFunction(FunctionRecord* fr) {
    // TODO set function prototype
    JsObject *obj;
    JsValue *val;
    jsValueCreatePointer(val, FUNCTION_TYPE, obj, OBJECT_VALUE_TYPE, sizeof(JsObject));
    obj->callInternal = fr;
    JS_SET_LITERAL(val, "prototype", objectCreatePlain());
    return val;
}

// https://www.ecma-international.org/ecma-262/5.1/#sec-9.9
static JsValue* coerceToObject(JsValue* val) {
    switch(jsValueType(val)) {
        case UNDEFINED_TYPE:
        case NULL_TYPE: {
            return NULL;
        }

        case OBJECT_TYPE:
        case FUNCTION_TYPE: {
            return val;
        }

        case NUMBER_TYPE:
        case BOOLEAN_TYPE:
        case STRING_TYPE: {
            fail("Unimplemented to coerce %s to object", gcObjectReflect((void*)val).name);
        }

        default: {
            fail("Unexpectedly got type %s for a JsValue",
                gcObjectReflect((void*)val).name);
        }
    }
    return NULL;
}

static JsValue* coerceForObjectReadWrite(JsValue* raw, const char* const verb, JsValue* property) {
    JsValue* coerced = coerceToObject(raw);
    if(coerced == NULL) {
        const char* const target = isUndefined(raw) ? "undefined" : "null";
        exceptionsThrowTypeError(
            stringCreateFromTemplate("Cannot %s property '%s' of %s", 
                verb, stringGetCString(property), target));
    }

    return coerced;
}


// used from compiled code
JsValue* objectGet(JsValue *rawVal, JsValue *name) {
    JsValue* val = coerceForObjectReadWrite(rawVal, "read", name);
    log_info("Getting %s", stringGetCString(val));
    JsValue* found = objectLookup(val, name);
    return found == NULL
      ? getUndefined()
      : found;
}

FunctionRecord* objectGetCallInternal(JsValue *val) {
    return OBJECT_VALUE(val)->callInternal;
}

static PropertyDescriptor* findProperty(PropertyDescriptor *pd, const char* const name) {
    while(pd != NULL) {
        if(strcmp(stringGetCString(pd->name), name) == 0) {
            return pd;
        }
        pd = pd->nextProperty;
    }
    return NULL;
}

JsValue* objectInternalOwnProperty(JsValue* value, JsValue* name) {
    const char* cString = stringGetCString(name);
    PropertyDescriptor* pd = findProperty(OBJECT_VALUE(value)->properties
            , cString);
    return pd == NULL
        ? NULL
        : pd->value;
}

ForOwnIterator objectForOwnPropertiesIterator(JsValue* value) {
    if(jsValueIsPrimitive(value)) {
        return (ForOwnIterator) { .property = NULL };
    }

    PropertyDescriptor* pd = OBJECT_VALUE(value)->properties;
    if(pd == NULL) {
        return (ForOwnIterator) { .property = NULL };
    } 

    return objectForOwnPropertiesNext((ForOwnIterator) {
        .next = pd,
    });
}

ForOwnIterator objectForOwnPropertiesNext(ForOwnIterator iterator) {
    if(iterator.next == NULL) {
        return (ForOwnIterator) { .property = NULL };
    } else {
        PropertyDescriptor* pd = iterator.next;
        return (ForOwnIterator) {
          .next = pd->nextProperty,
          .property = pd->name
        };
    }
}


// returns NULL or pointer to JsValue*
JsValue* objectLookup(JsValue *val, JsValue *name) {
    const char* cString = stringGetCString(name);

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
    return gcAllocate(sizeof(PropertyDescriptor), PROPERTY_DESCRIPTOR_TYPE);
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
JsValue* objectSet(JsValue* rawVal, JsValue* name, JsValue* value) {
    log_info("Setting %s in %s at %p", stringGetCString(name), jsValueReflect(rawVal).name, rawVal);
    JsValue* objectVal = coerceForObjectReadWrite(rawVal, "set", name);
    JsObject* object = jsValuePointer(objectVal);
    // this should be using the JS string value
    const char* nameString = stringGetCString(name);

    log_info("looking in %p for props", object->properties);
    PropertyDescriptor *descriptor = findProperty(object->properties, nameString);

    if(descriptor == NULL) {
        descriptor = propertyCreate();
        descriptor->name = name;
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
        cb(pd->name);
        cb(pd->value);
    }
}

char* objectDebug(JsValue* target) {
    JsObject* object = jsValuePointer(target);
    // TODO this can likely go away once arrays are in
    char* debugString = calloc(4096, 1);
    for(PropertyDescriptor* pd = object->properties;
        pd != NULL;
        pd = pd->nextProperty
    ) {
        char buffer[1024];
        sprintf(buffer, "%s:%s ", stringGetCString(pd->name), gcObjectReflect((void*)pd->value).name);
        strcat(debugString, buffer);
    }
    return debugString;
}

JsValue* objectInstanceof(JsValue* instance, JsValue* candidate) {
    if(jsValueType(candidate) != FUNCTION_TYPE) {
        exceptionsThrowTypeError(stringFromLiteral("Right hand side of 'instanceof' is not callable"));
    }
    JsValue* const candidatePt = JS_GET_LITERAL(candidate, "prototype");
    if(jsValueType(candidatePt) != OBJECT_TYPE) {
        exceptionsThrowTypeError(stringFromLiteral("Right hand side of 'instanceof' has non-object prototype property"));
    }

    do {
        instance = coerceToObject(instance);
        if(instance == NULL) {
            break;
        }
        JsValue* pt = OBJECT_VALUE(instance)->prototype;
        if(pt == candidatePt) {
            return getTrue();
        }

        if(pt == NULL || jsValueType(pt) == UNDEFINED_TYPE || jsValueType(pt) == NULL_TYPE) {
            break;
        }

        instance = pt;
    } while(instance != NULL);

    return getFalse();
}

JsValue* objectNewOperation(JsValue* function, JsValue* argumentValues[], uint64_t argumentCount) {
    JsValue* pt = JS_GET_LITERAL(function, "prototype");
    // TODO use Object.prototype if pt is undefined
    JsValue* this = objectCreate(pt);

    JsValue* returned = functionRunWithArguments(function, argumentValues, argumentCount, this);
    return jsValueIsPrimitive(returned)
        ? this
        : returned;
}

void objectDestroy() {
    // NOOP
}

