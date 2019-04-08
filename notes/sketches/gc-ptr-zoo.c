// variable - somewhere to store a value,
// either:
// - static/file scope
// - automatic - function scope
//
// pointer variables will point at
//
// 1. stack/automatic space
// 2. heap space (alloc'd)

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#define DEBUG_VAR(X, FMT) printf("%s var is "#FMT"\n", #X, X);
#define DEBUG_STATEMENT(X) printf(#X"\n"); X;
#define line(X) printf(X"\n");

typedef struct HeapValue {
    void* value;
} HeapValue;

typedef struct Version {
    int major;
    int minor;
    int patch;
} Version;

static HeapValue heap[1024];
static bool gcMarks[1024];

static HeapValue* someValuesOnHeap;

int space() {
    printf("\n");
}

int main() {
    // this automatic variables contains the number '7'
    int num = 7;
    DEBUG_VAR(num, %i);
    // this automatic variable contains a pointer to an in
    int* ptrToNum = &num;
    DEBUG_VAR(ptrToNum, %p);

    // This automatic variables containers a points to memory location
    // '7', which is garbage and likely will segfault.
    // Compiler will warn
    int* ptrToGarbage = num;
    DEBUG_VAR(ptrToGarbage, %p);

    // this automatic variable contains a pointer to the first
    // char in an array of 16 ints (which are initialized to 0
    char* charArray = calloc(sizeof(char), 16);
    charArray[2] = '\0';
    charArray[1] = 'i';
    charArray[0] = 'h';
    DEBUG_VAR(charArray, %s);
    DEBUG_VAR(charArray+1, %s);
    DEBUG_VAR(charArray, %p);
    DEBUG_VAR(charArray+1, %p);

    printf("Note: we stored the pointer in a automatic value which is itself in a location\n");
    DEBUG_VAR(&charArray, %p);

    space();
    line("For instance, after:");
    DEBUG_STATEMENT(charArray = &charArray[1]);
    line("We see");
    DEBUG_VAR(charArray, %s);
    line("If we did charArray = charArray[1] we'd be storing our second char's integer value as a pointer to a char!");
    
    int** ptrToPtr = &ptrToNum;
    DEBUG_VAR(**ptrToPtr, %i);

    int*** explodey = (void*)ptrToPtr;
    DEBUG_VAR(***explodey, %i);
}
