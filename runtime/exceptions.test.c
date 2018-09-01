#include <assert.h>
#include <stdbool.h>

#include "test.h"
#include "exceptions.h"
#include "objects.h"
#include "runtime.h"

static void itRunsTryByDefault() {
    Env* env = envCreateRoot();

    bool tryRun = false;
    if(exceptionsTry(env)) {
        tryRun = true;
    }

    assert(tryRun == true);
}

static void itRunsCatchWhenExceptionThrown() {
    Env* env = envCreateRoot();

    bool caught = false;
    if(exceptionsTry(env)) {
        exceptionsThrow(env, objectCreatePlain());
    } else {
        caught = true;
    }

    assert(caught == true);
}


int main() {
    testLanguageAndGcInit();

    test(itRunsTryByDefault);
    test(itRunsCatchWhenExceptionThrown);
}
