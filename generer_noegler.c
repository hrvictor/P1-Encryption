#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#define E 65537LL

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
long long int phi(long long int n);
long long int gcd(long long int num1, long long int num2);

int main(void) {
    srand((unsigned int)time(NULL));
    FILE *fptr;

    long long int p1 = 0, p2 = 0;
    int primeindex = 0;

    while (primeindex < 2) {
        long long int n = rand() % 1000000;      // random number
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

    printf("Prime nr. 1: %lld\n", p1);
    printf("Prime nr. 2: %lld\n", p2);

    long long n = p1 * p2;
    long long phi_n = phi(n);


    printf("n = %lld\n", n);
    printf("phi(n) = %lld\n", phi_n);

    fptr = fopen("keys.txt", "a");
    fprintf(fptr, "public key key: (n=%lld, e=%lld)\nprivate key: (n=%lld, d=)", n, E, n);
    fclose(fptr);

    long long int _gcd = gcd(E, phi_n);
    printf("gcd (e,phi(n)= %lld\n", _gcd);
    if(_gcd != 1){
      printf("Error: Choose new prime numbers\n");
      return 1;
    }

    return 0;
}
long long int phi(long long int n){
    long long int result = n; // Initialize result as n

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
long long int gcd(long long int num1, long long int num2) {
    // Base case: if the second number is 0, the first number is the GCD
    if (num2 == 0) {
        return num1;
    }
    // Recursive call with the second number and the remainder of num1 divided by num2
    return gcd(num2, num1 % num2);
}
