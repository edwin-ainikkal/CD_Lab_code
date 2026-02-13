#include <stdio.h>

int main() {

    int a = 5;
    int b = 10;
    int result;
    int i;
    int *ptr;

    ptr = &a;

    printf("Initial value of a: %d\n", a);
    printf("Value using pointer: %d\n", *ptr);

    scanf("%d", &b);

    result = a + b;

    if (result > 10 && a != b) {
        result += 2;
    } else {
        result -= 1;
    }

    for(i = 0; i < 5; i++) {
        result++;
    }

    while(result >= 0) {
        result--;
    }

    return 0;
}
