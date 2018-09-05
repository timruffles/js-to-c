#include <stdio.h>
#include <string.h>


static printMe(const char* const str) {
    printf("%p\n", str);
}

int main() {
    printMe("Hei");
    printMe("Hei");
    printMe("Hei 2");
    char* str = "hi";
    printMe(str);
    printf("%d\n", sizeof("hello"));
    printf("%d\n", sizeof("spo"));
}
