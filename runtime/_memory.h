#pragma once

#include <stdlib.h>
#include <assert.h>

#define ensureCallocBytes(V, M) V = calloc(1, M); assert(V != NULL);

