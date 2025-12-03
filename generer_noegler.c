#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#define E 65537LL
#define BLOCK_SIZE 2


int isPrime(int n);
long long int phi(long long int n);
long long int gcd(long long int num1, long long int num2);
long long int modInverse(long long int a, long long int m);
long long int extended_gcd(long long int a, long long int b,long long int *x, long long int *y);
long long int modInverse(long long int a, long long int m);
long long int modExp(long long int base, long long int exp, long long int mod);
long long int encrypt_char(unsigned char m, long long int n, long long int e);
unsigned char decrypt_char(long long int c, long long int n, long long int d);

int main(void) {
  srand((unsigned int)time(NULL));
  FILE *fptr;

  long long int p1 = 0, p2 = 0;
  int primeindex = 0;

  while (primeindex < 2) {
    long long int n = rand()%100000;      // random number
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
    if(_gcd != 1){
      printf("Error: Choose new prime numbers\n");
      return 1;
    }
  
  //stored in txt file as hex using the llx 
  fptr = fopen("keys.txt", "a");
  fprintf(fptr, "public key key: (n=%llx, e=%llx)\nprivate key: (n=%llx, d=%llx)", n, E, n, d);
  fclose(fptr);

  printf("This is the keypair saved to your file keys.txt, in hexadecimals:\npublic key key: (n=%llx, e=%llx)\nprivate key: (n=%llx, d=%llx)\n", n, E, n, d);

/*This part of the program with encryption and decryption works for a message up to 256 characters written in terminal
vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
    char message[256];

    printf("\nEnter a message to encrypt: ");

    // Read the line
    if (fgets(message, sizeof(message), stdin) == NULL) {
        printf("Error reading message.\n");
        return 1;
    }

    // Remove trailing newline from fgets, if present, and get length
    int len = 0;
    while (message[len] != '\0') {
        if (message[len] == '\n') {
            message[len] = '\0';
            break;
        }
        len++;
    }

    // If message length is odd, pad with '\0' so we always have full blocks
    if (len % BLOCK_SIZE != 0) {
        message[len] = '\0';
        len++;
    }

    int numBlocks = len / BLOCK_SIZE;
    long long int ciphertext[256];   // enough space (256 chars / 2 = 128 blocks max)

    // Encrypt in blocks of 2 characters
    for (int b = 0; b < numBlocks; b++) {
        long long int m = 0;

        // Pack 2 characters into one integer
        for (int j = 0; j < BLOCK_SIZE; j++) {
            unsigned char ch = (unsigned char)message[b * BLOCK_SIZE + j];
            m = m * 256 + ch;
        }

        ciphertext[b] = modExp(m, E, n);
    }

    printf("\nCiphertext blocks:\n");
    for (int b = 0; b < numBlocks; b++) {
        printf("%lld ", ciphertext[b]);
    }
    printf("\n");

    // Decrypt blocks back into characters
    char decrypted[256];

    for (int b = 0; b < numBlocks; b++) {
        long long int m = modExp(ciphertext[b], d, n);

        // Unpack 2 characters from the integer (reverse order)
        for (int j = BLOCK_SIZE - 1; j >= 0; j--) {
            decrypted[b * BLOCK_SIZE + j] = (char)(m % 256);
            m /= 256;
        }
    }

    // Terminate string at original logical length (len already includes padding if any)
    decrypted[len] = '\0';

    printf("\n=== RSA BLOCK STRING TEST ===\n");
    printf("Original : %s\n", message);
    printf("Decrypted: %s\n", decrypted);
/*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
This part of the program with encryption and decryption works for a message up to 256 characters written in terminal*/

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
// --------------------- extended_gcd ---------------------
// Extended Euclidean Algorithm.
// For given a and b, it computes:
//  - g = gcd(a, b)
//  - x and y such that: a*x + b*y = g
// x and y are "output parameters" returned through the pointers *x and *y.
long long int extended_gcd(long long int a, long long int b,long long int *x, long long int *y){
    if (b == 0){
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

/* Computes the modular inverse of a modulo m.
That is, finds x such that: a*x ≡ 1 (mod m).
Uses the extended Euclidean algorithm.
Returns the modular inverse if it exists, otherwise returns -1.*/
long long int modInverse(long long int a, long long int m){
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
long long int modExp(long long int base, long long int exp, long long int mod) {
    long long int result = 1 % mod;

    base = base % mod;

    while (exp > 0) {
        // If the current bit of exp is 1, multiply result by base (mod mod)
        if (exp & 1) {
          result = (result * base) % mod;
        }

        base = base * base % mod; 

        // Shift exponent right by 1 bit (divide by 2)
        exp = exp / 2; 
    }

    return result;
}
// Encrypt a single byte (character)
long long int encrypt_char(unsigned char m, long long int n, long long int e) {
  return modExp(m, e, n);
}

// Decrypt a single ciphertext value
unsigned char decrypt_char(long long int c, long long int n, long long int d) {
  long long int m = modExp(c, d, n);
    return (unsigned char)m;
}