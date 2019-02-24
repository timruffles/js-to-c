#include <stdio.h>
#include <assert.h>
#include <strings.h>
#include <stdlib.h>

#include "global.h"
#include "environments.h"
#include "strings.h"
#include "functions.h"
#include "objects.h"
#include "lib/debug.h"

static FILE* outStream;

#define ONE_MIBI 1048576
static char outputBuffer[ONE_MIBI];

static FILE* getOutStream() {
    if(outStream == NULL) {
        return fopen("/dev/stdout", "w");
    }
    return outStream;
}

extern void _setOutStream(FILE* stream) {
    outStream = stream;
}

static void ensureWrite(char* output, FILE* stream) {
    size_t toWrite = strlen(output);
    size_t written = fwrite(output, sizeof(char), toWrite, stream);
    assert(written == toWrite);
}

JsValue* consoleLog(Env* env) {
    // TODO #varargs
    FILE* stream = getOutStream();
    assert(stream != NULL);

    // TODO use proper ToString operation
    jsValueToCString(envGet(env, stringFromLiteral("arg0")), outputBuffer, sizeof(outputBuffer));
    ensureWrite(outputBuffer, stream);
    ensureWrite("\n", stream);
    fflush(stream);

    return getUndefined();
}


// creates & sets the global env (note: called only once
// except for in tests)
JsValue* createGlobalObject() {
    JsValue* global = objectCreatePlain();
    Env* globalEnv = envFromGlobal(global);

    JsValue* console = objectCreatePlain();
    JsValue* str = stringFromLiteral("console");
    objectSet(global, str,
            console);

    // TODO varargs
    JsValue** consoleArgs = calloc(sizeof(JsValue*), 1);
    consoleArgs[0] = stringFromLiteral("arg0");
    JsValue* consoleLogJsv = functionCreate(consoleLog, consoleArgs, 1, globalEnv);
    log_info("log fn %p env %p", consoleLogJsv, globalEnv);
    objectSet(console, stringFromLiteral("log"),
            consoleLogJsv);

    JS_SET_LITERAL(global, "NaN", getNaN());
    
    // setup reference to self (important when used as an env)
    objectSet(global, stringFromLiteral("global"),
            global);

    return global;
}
