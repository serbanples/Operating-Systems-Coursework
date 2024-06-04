#include <stdio.h>

long factorial(int n) {
    long result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

int main() {
    int n = 1000;
    printf("The factorial of %d is %ld\n", n, factorial(n));
    return 0;
}
