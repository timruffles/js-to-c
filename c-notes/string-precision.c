#include <stdio.h>
#include <stdlib.h>


static char ten[] = "abcdefghij";
static char* five;
static char* fiveB;

int main() {
    printf("%s\n", ten);
    printf("%.5s\n", ten);

    // removing this will cause a segfault as five
    // is a NULL pointer
    five = calloc(sizeof(char), 6);
    fiveB = calloc(sizeof(char), 6);

    sprintf(five, "%.5s", ten);
    printf("%s\n", five);

    // TODO - why does this 
    snprintf(fiveB, 6, "%s", ten);
    printf("%s\n", fiveB);
}
