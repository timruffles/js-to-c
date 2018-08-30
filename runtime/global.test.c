#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "test.h"
#include "environments.h"
#include "global.h"
#include "strings.h"
#include "objects.h"
#include "gc.h"

static void itCreatesTheGlobalObject() {
    JsValue* global = createGlobalObject();
    assert(global != NULL);
}

static void itLogsJsValuesToOutput() {
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
    _functionRun(consoleLogJsv, env);
}

static void itWorksWithEnv() {
    JsValue* global = createGlobalObject();
    Env* env = envFromGlobal(global);

    JsValue* console = envGet(env, stringCreateFromCString("console"));
    assert(console != NULL);
}

int main() {
    testLanguageAndGcInit();

    FILE* fp = fopen("/tmp/globaltest", "w+");
    assert(fp != NULL);
    _setOutStream(fp);

    test(itCreatesTheGlobalObject);
    test(itLogsJsValuesToOutput);

    test(itWorksWithEnv);
}

