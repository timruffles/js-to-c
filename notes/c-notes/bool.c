#include <stdio.h>
#include <stdbool.h>

typedef struct Marks {
    bool a: 1;
    bool b: 1;
    bool c: 1;
    bool d: 1;
    bool e: 1;
    bool f: 1;
    bool g: 1;
    bool h: 1;
} Marks;

char chars[94];
bool marks[94];
Marks markStructs[94];

int main() {
    printf("%lu %lu %lu \n", sizeof(marks), sizeof(chars), sizeof(markStructs));
    printf("%p \n", marks + 1);
    printf("%p \n", marks + 2);
    printf("%p \n", markStructs + 1);
    printf("%p \n", markStructs + 2);
}
