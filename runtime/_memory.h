#pragma once

#include <stdlib.h>
#include <assert.h>

#define ensureCallocBytes(V, S) V = calloc(1, S); assert(V != NULL);

