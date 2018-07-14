#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "exceptions.h"

void throwError(char* message) {
    printf("Your program caused an error to be thrown ('%s'), but they are not implemented yet.", message);
    exit(1);
}
