#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "language.h"
#include "lib/debug.h"
#include "gc.h"
#include "array.h"
#include "strings.h"
#include "objects.h"
#include "_memory.h"

typedef struct ArrayDetails {
    GcHeader;

    uint64_t length;
} ArrayDetails;

JsValue* arrayCreate(uint64_t length) {
  ObjectValueCreation cr = objectCreateArray(length);
  return cr.value;
}

// used by compiler to init a pre-lengthed array
void arrayInitialiseIndex(JsValue* array, JsValue* index, JsValue* val) {
    objectSet(array, index, val);
}

JsValue* arrayPutInternal(JsValue* ar, JsValue* key, JsValue* value) {
    char* prop = stringGetCString(key);
    if(strcmp(prop, "length") == 0) {
        // TODO
        return getUndefined();
    }

    // TODO - overflow etc
    char* terminator;
    errno = 0;
    int64_t index = strtoll(prop, &terminator, 10);
    log_info("array set %s %i", prop, index);
    if(errno == 0) {
        objectArrayBumpLength(ar, index);
    }

    return objectPut(ar, key, value);
}

JsValue* arrayGetInternal(JsValue* object, JsValue* nameString) {
    // intern length
    char* prop = stringGetCString(nameString);
    if(strcmp(prop, "length") == 0) {
        return objectArrayLength(object);
    } else {
        return objectGetInternal(object, nameString);
    }
}
