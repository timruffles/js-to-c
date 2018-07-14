#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "test.h"
#include "environments.h"
#include "global.h"
#include "strings.h"
#include "objects.h"

void itCreatesTheGlobalObject() {
    JsValue* global = createGlobalObject();
    assert(global != NULL);
}

void itLogsJsValuesToOutput() {
    JsValue* global = createGlobalObject();
    Env* env = envCreateRoot();
    JsValue* name = stringCreateFromCString("arg0");
    envDeclare(env, name);
    envSet(env, name, getTrue());

    JsValue* consoleLogJsv = objectGet(
        objectGet(global,
            stringCreateFromCString("console")),
        stringCreateFromCString("log")
    );
    functionRun(consoleLogJsv, env);
}

int main() {
    //FILE* fp = fopen("/tmp/globaltest", "w+")
    //assert(fp != NULL);
    //setOutStream(fp);

    test(itCreatesTheGlobalObject);
    test(itLogsJsValuesToOutput);
}

