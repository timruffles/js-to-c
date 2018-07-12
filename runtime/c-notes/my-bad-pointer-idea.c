#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

/**
 * So, plan was to store the values for the JS compiler
 * in a union, nicking Lua's idea. That way immediate
 * values like numbers live inside the JSValue wrapper,
 * while the compound values lived elsewhere:
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
} JsValueContent;

typedef struct {
    // lets us discriminate the union
    TypeTag type;
    JsValueContent value;
} JsValue;


/**
 * So I thought I could be clever and have a single
 * valueGet function that'd return the N bytes of the 
 * union. Now - since then I've decided that's a silly
 * idea and having the JsValue module expose typed getters
 * makes more sense - but I'm interested in that it wasn't doable.
 *
 * My first though was to both pass in and return the union
 * as a uintptr_t - as it was going to be as wide as a single
 * pointer:
 */


typedef uintptr_t OpaqueValue;

JsValue* jsValueCreate(TypeTag, OpaqueValue);
JsValue* jsValueCreate(TypeTag tag, OpaqueValue value) {
    JsValue* val = calloc(sizeof(JsValue), 1);
    *val = (JsValue) {
        .type = tag,
    };
    val->value.object = (void*)value;
    return val;
}

OpaqueValue jsValueGet(JsValue*);
OpaqueValue jsValueGet(JsValue* value) {
    return (OpaqueValue) value->value.object;
}

void printBytes(char* addressOfSomething, uint64_t bytes) {
    printf("Starting at %#x, bytes: ", addressOfSomething);
    for(uint64_t i = 0; i < bytes; i++) {
        uint8_t byte = *(addressOfSomething + i);
        printf("%#x ", byte);
    }
    printf("\n");
} 



int main() {
    assert(sizeof(JsValueContent) == sizeof(void *));

    printf("Ok - we're going to store the string 'lols', then print its bytes\n");
    double pretendObject = 4242;
    printBytes(&pretendObject, sizeof(void *));
    printf("Next we're going to store it in a union, and do the same\n");
    JsValueContent content = { .number = pretendObject };
    printBytes(&content, sizeof(void *));

    printf("Then, take the union and pop it into a uintptr_t");
    char* addressOfUnion = &content;
    uintptr_t data = addressOfUnion;
    printBytes(data, sizeof(void *));

    printf("The, take that uintptr_t and turn it into a double\n");
    char* asWhatever = &data;
    double doubleAgain = *asWhatever;
    printBytes(&doubleAgain, sizeof(void *));


    JsValue* val1 = jsValueCreate(NUMBER, 42.0);
    JsValue* val2 = jsValueCreate(BOOLEAN, TRUE);
    JsValue* val3 = jsValueCreate(OBJECT, &pretendObject);

    printf("val1 %f - \n", jsValueGet(val1)); 
    printf("val2 %c\n", (char)jsValueGet(val2)); 

    int* restoredPointer = (int*)jsValueGet(val3);
    printf("*val3 %i\n", *restoredPointer); 
}
