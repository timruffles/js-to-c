#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>

/**
 * So, plan is to store the values for the JS compiler
 * in a union, nicking Lua's idea. That way immediate
 * values like numbers live inside the JSValue wrapper,
 * while the compound values lived elsewhere.
 */

typedef char* TypeTag;
typedef char BooleanValue;
static const char TRUE = 'Y';

static char OBJECT[] = "object";
static char BOOLEAN[] = "boolean";
static char NUMBER[] = "number";

typedef union {
    void* object;
    double number;
    BooleanValue boolean;

    uintptr_t raw;
} JsValueContent;

typedef struct {
    // to discriminate the union
    TypeTag type;
    JsValueContent value;
} JsValue;

// some inspection
void printBytes(void*, uint64_t);
void printBytes(void* addressOfSomething, uint64_t bytes) {
    char* charPtr = addressOfSomething;
    printf("Starting at %p, bytes: ", (void*)charPtr);
    for(uint64_t i = 0; i < bytes; i++) {
        int8_t byte = *(charPtr + i);
        printf("%#x ", byte);
    }
    printf("\n");
} 


/**
 * So I thought I could be clever and have a single
 * valueGet function that'd return the N bytes of the 
 * union. Now - since then I've decided that's a silly
 * idea and having the JsValue module expose typed getters
 * makes more sense - but I'm interested in that it wasn't doable.
 *
 * My first though was to both pass in and return the union
 * as a uintptr_t - as it was going to be as wide as a 
 * pointer.
 *
 * I noticed two things:
 * - this makes for a LOT of ugly casting to make the compiler
 *   stop pointing out it's a bad idea
 * - it's easy to break things if you do casts that will cause
 *   data conversion (especially double -> uintptr_t) rather than
 *   simply a view of a different type of the same data
 */


typedef uintptr_t OpaqueValue;

JsValue* jsValueCreate(TypeTag, uintptr_t);
JsValue* jsValueCreate(TypeTag tag, uintptr_t value) {
    JsValue* val = calloc(sizeof(JsValue), 1);
    *val = (JsValue) {
        .type = tag,
    };
    // Although the union elements are of different sizes
    // unions' elements are all aligned to address of union,
    // so we can just read the max number of bytes for every
    // argument.
    //
    // Argument that this is ok: char is well defined as a single
    // byte, so having 7 bytes of garbage after is not a problem.
    //
    // Pointers and doubles both take 8 bytes, so we're never
    // getting any garbage anyway.
    //
    //     jsValueCreate(.., 'Y')
    //
    //     jsValueCreate(.., 47.0)
    //
    //     jsValueCreate(.., somePtr)
    //
    //memcpy(&val->value, &value, sizeof(JsValueContent));
    val->value.raw = value;
    return val;
}

uintptr_t jsValueGet(JsValue*);
uintptr_t jsValueGet(JsValue* value) {
    printBytes(&value->value, sizeof(JsValueContent));
    return (uintptr_t)value->value.object;
}

void manuallyMessingAround(void);
void manuallyMessingAround() {
    // check our assumptions - the union is as wide as a ptr value
    assert(sizeof(JsValueContent) == sizeof(void *));

    printf("\nPart One\n\n");
    printf("Ok - we're going to store the double 4242, then print its bytes\n");
    double pretendObject = 4242;
    printBytes(&pretendObject, sizeof(void *));

    printf("\nNext we're going to store it in a union, and do the same\n");
    JsValueContent content = { .number = pretendObject };
    printBytes(&content, sizeof(void *));

    printf("\nThen, take the union and pop it into a uintptr_t\n");
    uintptr_t* ptr = (void *)(&content);
    uintptr_t got_it_a = *ptr;

    // this isn't safe - as it reverses the direction with the cast
    // uintptr_t one_liner = (uintptr_t)(void *)(&content);

    // squash the original variable just to make it ultra clear
    // we have got the same bytes in got_it as we had in the location
    // of content
    uintptr_t got_it = got_it_a;
    got_it_a = 7;

    printBytes(&got_it, sizeof(void *));

    printf("\nThen, take that uintptr_t and turn it into a double\n");
    void* asWhatever = &got_it;
    double* doublePtr = asWhatever;
    double doubleAgain = *doublePtr;
    printBytes(&doubleAgain, sizeof(void *));

    printf("\nFinally, print the location of all of the copies of our data, and their values\n\n");

    printf("pretendObject - ");
    printBytes(&pretendObject, sizeof(void *));

    printf("content - ");
    printBytes(&content, sizeof(void *));

    printf("got_it - ");
    printBytes(&got_it, sizeof(void *));

    printf("doubleAgain - ");
    printBytes(&doubleAgain, sizeof(void *));
}

void jsValueTests(void);
void jsValueTests() {
    double pretendObject = 4242;
    printf("\n\n\nPart Two\n\n");
    double doubleVal = 47;
    JsValue* val1 = jsValueCreate(NUMBER, *(uintptr_t*)(void*)&doubleVal);
    JsValue* val2 = jsValueCreate(BOOLEAN, TRUE);
    JsValue* val3 = jsValueCreate(OBJECT, &pretendObject);

    uintptr_t r = jsValueGet(val1);

    // copy out our double value into another double variable
    double dblRestored = *(double*)(void*)&r;
    assert(dblRestored - doubleVal < 0.0000000001);
    printf("val1 %f - \n", dblRestored);

    // read out the char
    char readChar = (char)jsValueGet(val2);
    assert(readChar == TRUE);
    printf("val2 %c\n", readChar); 

    // read out the pointer value
    void* restoredPointer = jsValueGet(val3);
    assert(restoredPointer == &pretendObject);
    printf("*val3 %p\n", restoredPointer); 
}

int main() {
    manuallyMessingAround();
    jsValueTests();
}
