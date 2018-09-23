#include <stdio.h>
#include <string.h>


int main() {
    int env = 1;
    {
        int env = env + 2;
        printf("%i", env);
    }
}
