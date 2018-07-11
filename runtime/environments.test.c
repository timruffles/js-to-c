#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "environments.h"

#define test(T) (T)(); printf("%s passed", #T);

void itCanCreateRootEnv() {
    Env* env = EnvCreateRoot();
    assert(env != NULL);
    free(env);
}

int main(int argc, char** argv) {
    test(itCanCreateRootEnv);
}

