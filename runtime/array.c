#include <stdint.h>

#include "language.h"
#include "gc.h"
#include "array.h"
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
