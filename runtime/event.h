#pragma once

#include "language.h"
#include "runtime.h"

void eventInit(RuntimeEnvironment*);
void eventLoop(void);
void eventTimeout(JsValue* fn, FunctionArguments args, uint64_t ms);
