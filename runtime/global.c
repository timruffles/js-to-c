#include <stdio.h>
#include <assert.h>
#include <strings.h>
#include <stdlib.h>

#include "environments.h"
#include "strings.h"
#include "functions.h"
#include "objects.h"

FILE* outStream;

FILE* getOutStream() {
    if(outStream == NULL) {
        return fopen("/dev/stdout", "w");
    }
    return outStream;
}

void setOutStream(FILE* stream) {
    outStream = stream;
}

void ensureWrite(char* output, FILE* stream) {
    size_t toWrite = strlen(output);
    size_t written = fwrite(output, sizeof(char), toWrite, stream);
    assert(written == toWrite);
}

JsValue* consoleLog(Env* env) {
    // TODO varags
    FILE* stream = getOutStream();
    assert(stream != NULL);

    char* output = jsValueToString(envGet(env, stringCreateFromCString("arg0")));
    ensureWrite(output, stream);
    ensureWrite("\n", stream);
    fflush(stream);

    return getUndefined();
}

JsValue* createGlobalObject() {
    JsValue* global = objectCreatePlain();

    JsValue* console = objectCreatePlain();
    objectSet(global, stringCreateFromCString("console"),
            console);

    JsValue** consoleArgs = calloc(sizeof(JsValue*), 1);
    consoleArgs[0] = stringCreateFromCString("arg0");
    JsValue* consoleLogJsv = functionCreate(consoleLog, consoleArgs, 1);
    objectSet(console, stringCreateFromCString("log"),
            consoleLogJsv);

    // setup reference to self (important when used as an env)
    objectSet(global, stringCreateFromCString("global"),
            global);

    return global;
}

