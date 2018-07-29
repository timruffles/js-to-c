#include <stdio.h>

#include "language.h"
#include "environments.h"

JsValue* createGlobalObject(void);
JsValue* consoleLog(Env*);

void _setOutStream(FILE*);
