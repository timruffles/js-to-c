#include <stdio.h>

extern void hi() {
    // try changing me, recompiling the dynlib, and running an already compiled
    // client program. it'll see the new version of this code
    printf("hi\n");
}
