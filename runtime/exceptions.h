#pragma once
#include <assert.h>
#include <setjmp.h>

#include "language.h"
#include "environments.h"

#define exceptionsTry(E) (exceptionsTryStart(E), setjmp(exceptionsJumpBuf) == 0)

typedef struct JsCatch JsCatch;

extern jmp_buf exceptionsJumpBuf;

void exceptionsThrow(Env* env, JsValue* error);

void exceptionsCatchStart(Env* env);
void exceptionsCatchEnd(void);

void exceptionsThrowReferenceError(Env* env, JsValue* name);

void exceptionsTryStart(Env* env);
