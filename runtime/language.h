#pragma once

#include "stdint.h"

typedef struct {
   char * content;
   uint64_t length;
} JsString;


typedef JsString JsIdentifier;

// TODO
typedef void* JsValue;
