#include <stdio.h>
#include <stdlib.h>



typedef union {
    int* pointerValue;
    double doubleValue;
} TheUnion;

int main() {
    printf("Size of union<int* | double> %zu\n", sizeof(TheUnion));

    TheUnion a = {
        .doubleValue = 3,
    };

    printf("A double value as unsigned ptr %lu\n", a.doubleValue);
    printf("A double value as double %f\n", a.doubleValue);

    uintptr_t d = a.doubleValue;
    printf("Extract double into ptr, format as double %f\n", d);

}
