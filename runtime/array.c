#include <stdint.h>
#include <string.h>

#include "language.h"
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
    return getNull();
}

JsValue* arrayGetInternal(JsValue* object, JsValue* nameString) {
    char* prop = stringGetCString(nameString);
    if(strcmp(prop, "length") == 0) {
        return objectArrayLength(object);
    } else {
        return objectGetInternal(object, nameString);
    }
}
