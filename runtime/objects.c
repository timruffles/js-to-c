#include "language.h"

typedef struct {
    JsIdentifier *id;
    JsValue *value;

    UT_hash_handle hh;
} PropertyDescriptor;

typedef struct {
    PropertyDescriptor* descriptors;
    JsObject* prototype;
} JsObject;

const JsValue objectPrototype = (JsValue) {
  .type = OBJECT_TYPE,
  .value = {
      .object = (JsObject) {
          .descriptors = NULL,
          .prototype = UNDEFINED,
      },
  },
};

/**
 * A 'plain' object - with Object as prototype
 */
JsValue* objectCreatePlain() {
    JsValue *val = calloc(sizeof(JsValue), 1);
    JsObject *obj = calloc(sizeof(JsObject), 1);
    *val = &(JsValue) {
            .type = OBJECT_TYPE,
            .value = {
                .object = obj,
            },
    };
    return val;
}
