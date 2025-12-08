#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define E 65537LL


int isPrime(int n);
long long int phi(long long int n);
long long int gcd(long long int num1, long long int num2);
long long int modInverse(long long int a, long long int m);
long long int extended_gcd(long long int a, long long int b, long long int* x, long long int* y);
long long int modInverse(long long int a, long long int m);

int generate_keys(void) {
    srand((unsigned int)time(NULL));
    FILE* fptr;

    long long int p = 0, q = 0;
    int primeindex = 0;

    while (primeindex < 2) {
        long long int n = rand();      // random number
        if (isPrime(n)) {
            if (primeindex == 0) {
                p = n;
            }
            else {
                q = n;
            }
            primeindex++;
        }
    }

    printf("Prime nr. 1: %lld\n", p);
    printf("Prime nr. 2: %lld\n", q);

    long long n = p * q;
    long long phi_n = phi(n);
    // Compute d
    long long int d = modInverse(E, phi_n);
    if (d == -1) {
        printf("Error: modular inverse does not exist\n");
        return 1;
    }


    printf("n = %lld\n", n);
    printf("phi(n) = %lld\n", phi_n);
    printf("d = %lld\n", d);

    long long int _gcd = gcd(E, phi_n);
    printf("gcd (e,phi(n)= %lld\n\n", _gcd);
    if (_gcd != 1) {
        printf("Error: Choose new prime numbers\n");
        return 1;
    }

    //stored in txt file as hex using the llx 
    fptr = fopen("keys.txt", "a");
    fprintf(fptr, "public key key: (n=%llx, e=%llx)\nprivate key: (n=%llx, d=%llx)", n, E, n, d);
    fclose(fptr);

    printf("This is the keypair saved to your file keys.txt, in hexadeximals:\npublic key key: (n=%llx, e=%llx)\nprivate key: (n=%llx, d=%llx)\n", n, E, n, d);

    return 0;
}
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
long long int phi(long long int n) {
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
// --------------------- extended_gcd ---------------------
// Extended Euclidean Algorithm.
// For given a and b, it computes:
//  - g = gcd(a, b)
//  - x and y such that: a*x + b*y = g
// x and y are "output parameters" returned through the pointers *x and *y.
long long int extended_gcd(long long int a, long long int b, long long int* x, long long int* y) {
    if (b == 0) {
        *x = 1;
        *y = 0;
        return a;
    }
    long long int x1, y1;
    long long int g = extended_gcd(b, a % b, &x1, &y1);
    *x = y1;
    *y = x1 - (a / b) * y1;

    return g;
}
// --------------------- modInverse ---------------------
// Computes the modular inverse of a modulo m.
// That is, finds x such that: a*x ≡ 1 (mod m).
// Uses the extended Euclidean algorithm.
// Returns the modular inverse if it exists, otherwise returns -1.
long long int modInverse(long long int a, long long int m) {
    long long int x, y;
    long long int g = extended_gcd(a, m, &x, &y);

    if (g != 1) {
        // inverse does not exist
        return -1;
    }

    // x might be negative, so make it positive modulo m
    long long int res = x % m;
    if (res < 0) res += m;
    return res;
}