#include <stdio.h>
void foo() {
    return;
}

int main() {
    int x = 0;
    foo();
    printf("Hello, World! %d", x);
    return 0;
}
