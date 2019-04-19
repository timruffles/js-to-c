#pragma once
#include <assert.h>
#include <setjmp.h>

#include "language.h"
#include "gc.h"
#include "environments.h"

#define exceptionsTry(E) (exceptionsCatchStart(E), setjmp(exceptionsJumpBuf) == 0)

typedef struct JsCatch JsCatch;

extern jmp_buf exceptionsJumpBuf;

void exceptionsThrow(JsValue* error);
JsCatch* exceptionsRootCatchCreate(void);

void exceptionsCatchStart(Env* env);
void exceptionsCatchEnd(void);

void exceptionsThrowReferenceError(JsValue* msg);
void exceptionsThrowTypeError(JsValue* msg);

// owned by gc module
void _exceptionsGcProtect(JsValue*);
void _exceptionsGcUnprotectAfterThrow(void);
void _exceptionsGcUnprotect(void);
void _exceptionsGcForeachValue(GcCallback*);
