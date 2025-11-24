#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int isPrime(int n) {
    if (n <= 1) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;

    for (int i = 3; i * i <= n; i += 2) {
        if (n % i == 0)
            return 0;
    }
    return 1;
}
int phi(int n);
int gcd(int num1, int num2);

int main(void) {
    srand((unsigned int)time(NULL));
    FILE *fptr;

    int p1 = 0, p2 = 0;
    int primeindex = 0;

    while (primeindex < 2) {
        int n = rand() % 100000;      // random number
        if (isPrime(n)) {
            if (primeindex == 0) {
                p1 = n;
            }
            else {
                p2 = n;
            }
            primeindex++;
        }
    }

    printf("Prime nr. 1: %d\n", p1);
    printf("Prime nr. 2: %d\n", p2);

    int n = p1 * p2;
    int phi_n = phi(n);

    printf("n = %d\n", n);
    printf("phi(n) = %d\n", phi_n);

    fptr = fopen("keys.txt", "a");
    fprintf(fptr, "%d\n %d", n, phi_n);
    fclose(fptr);

    int _gcd = gcd(65537, (p1-1)*(p2-1));
    printf("gcd er %d\n", _gcd);




    return 0;
}
int phi(int n){
    int result = n; // Initialize result as n

    // Iterate through all numbers from 2 up to sqrt(n)
    for (int i = 2; i * i <= n; i++) {
        // If i is a prime factor of n
        if (n % i == 0) {
            // While i divides n, divide n by i and update the result
            while (n % i == 0) {
                n /= i;
            }
            result -= result / i;
        }
    }

    // If n has a prime factor greater than sqrt(n) (there can be at most one such factor)
    if (n > 1) {
        result -= result / n;
    }
    return result;
}
int gcd(int num1, int num2) {
    // Base case: if the second number is 0, the first number is the GCD
    if (num2 == 0) {
        return num1;
    }
    // Recursive call with the second number and the remainder of num1 divided by num2
    return gcd(num2, num1 % num2);
}
