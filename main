#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define E 65537LL
#define MAX_MESSAGE 1024
#define MAX_BLOCKS  256   // max number of RSA blocks

#pragma warning(disable : 4996)

#include "bn.h"  // tiny-bignum-c header

/* ---------- function prototypes ---------- */

int isPrime(int n);
long long int phi_ll(long long int n);
long long int gcd_ll(long long int num1, long long int num2);
long long int extended_gcd_ll(long long int a, long long int b, long long int* x, long long int* y);
long long int modInverse_ll(long long int a, long long int m);

/* big-number modular exponentiation with long long exponent */
void modExp_bn_ll(struct bn* base, long long exp, struct bn* mod, struct bn* result);

/* encode message into RSA blocks (base-256) */
int message_to_blocks(const char* msg,
    struct bn* blocks,
    int max_blocks,
    int* out_blocks,
    struct bn* modulus);

/* decode RSA blocks back to message */
int block_to_bytes(struct bn* in, char* out, int maxlen);

/* ---------- main ---------- */

int main(void) {

    srand((unsigned int)time(NULL));
    FILE* fptr;

    long long int p1 = 0, p2 = 0;
    int primeindex = 0;

    /* generate two random primes (small, but fine for demo) */
    while (primeindex < 2) {
        long long int candidate = rand() % 100000;      // random number
        if (candidate < 1000) continue;                 // avoid tiny primes
        if (isPrime((int)candidate)) {
            if (primeindex == 0) {
                p1 = candidate;
            }
            else {
                p2 = candidate;
            }
            primeindex++;
        }
    }

    printf("Prime nr. 1: %lld\n", p1);
    printf("Prime nr. 2: %lld\n", p2);

    /* long long modulus and phi (for keygen only) */
    long long n_ll = p1 * p2;
    long long phi_ll_val = phi_ll(n_ll);

    /* big-number versions of key components */
    struct bn bn_p1, bn_p2, bn_n, bn_phi, bn_d;

    bignum_init(&bn_p1);
    bignum_init(&bn_p2);
    bignum_init(&bn_n);
    bignum_init(&bn_phi);
    bignum_init(&bn_d);

    bignum_from_int(&bn_p1, p1);
    bignum_from_int(&bn_p2, p2);
    bignum_from_int(&bn_n, n_ll);
    bignum_from_int(&bn_phi, phi_ll_val);

    /* compute d (still using long long math) */
    long long d_ll = modInverse_ll(E, phi_ll_val);
    if (d_ll == -1) {
        printf("Error: modular inverse does not exist\n");
        return 1;
    }
    bignum_from_int(&bn_d, d_ll);

    printf("n (ll)      = %lld\n", n_ll);
    printf("phi(n) (ll) = %lld\n", phi_ll_val);
    printf("d (ll)      = %lld\n", d_ll);

    long long int _gcd = gcd_ll(E, phi_ll_val);
    printf("gcd(e, phi(n)) = %lld\n\n", _gcd);
    if (_gcd != 1) {
        printf("Error: Choose new prime numbers\n");
        return 1;
    }

    /* store keys in file as hex */
    fptr = fopen("keys.txt", "a");
    if (fptr != NULL) {
        fprintf(fptr, "public key:  (n=%llx, e=%llx)\nprivate key: (n=%llx, d=%llx)\n\n",
            n_ll, E, n_ll, d_ll);
        fclose(fptr);
    }

    printf("Keypair saved to keys.txt, in hexadecimals:\n"
        "public key:  (n=%llx, e=%llx)\n"
        "private key: (n=%llx, d=%llx)\n",
        n_ll, E, n_ll, d_ll);

    /* ---------- read message ---------- */

    char message[MAX_MESSAGE];

    printf("\nEnter a message to encrypt: ");

    if (fgets(message, sizeof(message), stdin) == NULL) {
        printf("Error reading message.\n");
        return 1;
    }

    /* Remove newline from fgets, if present */
    {
        int len = 0;
        while (message[len] != '\0') {
            if (message[len] == '\n') {
                message[len] = '\0';
                break;
            }
            len++;
        }
    }

    if (message[0] == '\0') {
        printf("Empty message, nothing to encrypt.\n");
        return 0;
    }

    /* ---------- encode message into bignum blocks ---------- */

    struct bn m_blocks[MAX_BLOCKS];
    int m_block_count = 0;

    int enc_ok = message_to_blocks(message, m_blocks, MAX_BLOCKS,
        &m_block_count, &bn_n);
    if (enc_ok != 0) {
        printf("Error: Message too long for given modulus / block limits.\n");
        return 1;
    }

    printf("\nMessage encoded into %d block(s).\n", m_block_count);

    /* ---------- encrypt each block: c_i = m_i^E mod n ---------- */

    struct bn c_blocks[MAX_BLOCKS];
    for (int i = 0; i < m_block_count; ++i) {
        bignum_init(&c_blocks[i]);
        modExp_bn_ll(&m_blocks[i], E, &bn_n, &c_blocks[i]);
    }

    printf("\nCiphertext blocks (hex):\n");
    for (int i = 0; i < m_block_count; ++i) {
        char buf[1024];
        bignum_to_string(&c_blocks[i], buf, sizeof(buf));
        printf("C[%d] = %s\n", i, buf);
    }

    /* ---------- decrypt each block: m_i' = c_i^d mod n ---------- */

    struct bn m_dec_blocks[MAX_BLOCKS];
    for (int i = 0; i < m_block_count; ++i) {
        bignum_init(&m_dec_blocks[i]);
        modExp_bn_ll(&c_blocks[i], d_ll, &bn_n, &m_dec_blocks[i]);
    }

    /* ---------- convert decrypted blocks back to message ---------- */

    char decrypted[MAX_MESSAGE];
    int offset = 0;

    for (int i = 0; i < m_block_count; ++i) {
        int remaining = MAX_MESSAGE - 1 - offset; // leave space for final '\0'
        if (remaining <= 0) {
            printf("Error: decrypted message buffer too small.\n");
            return 1;
        }

        int bytes = block_to_bytes(&m_dec_blocks[i],
            decrypted + offset,
            remaining + 1);  // +1 because block_to_bytes writes its own '\0'

        if (bytes < 0) {
            printf("Error converting decrypted block %d back to bytes.\n", i);
            return 1;
        }

        offset += bytes;
    }

    decrypted[offset] = '\0';

    printf("\nDecrypted message: %s\n", decrypted);

    return 0;
}

/* ---------- helpers: small-int number theory ---------- */

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

/* phi for long long (used only for small keygen) */
long long int phi_ll(long long int n) {
    long long int result = n;

    for (long long int i = 2; i * i <= n; i++) {
        if (n % i == 0) {
            while (n % i == 0) {
                n /= i;
            }
            result -= result / i;
        }
    }

    if (n > 1) {
        result -= result / n;
    }
    return result;
}

long long int gcd_ll(long long int num1, long long int num2) {
    if (num2 == 0) {
        return num1;
    }
    return gcd_ll(num2, num1 % num2);
}

long long int extended_gcd_ll(long long int a, long long int b,
    long long int* x, long long int* y) {
    if (b == 0) {
        *x = 1;
        *y = 0;
        return a;
    }
    long long int x1, y1;
    long long int g = extended_gcd_ll(b, a % b, &x1, &y1);
    *x = y1;
    *y = x1 - (a / b) * y1;

    return g;
}

long long int modInverse_ll(long long int a, long long int m) {
    long long int x, y;
    long long int g = extended_gcd_ll(a, m, &x, &y);

    if (g != 1) {
        return -1;
    }

    long long int res = x % m;
    if (res < 0) res += m;
    return res;
}

/* ---------- big-number helpers ---------- */

/* Modular exponentiation with big-number base, modulus, and long long exponent:
   result = base^exp mod mod */
void modExp_bn_ll(struct bn* base, long long exp, struct bn* mod, struct bn* result) {
    struct bn res;
    struct bn b;
    struct bn tmp;

    bignum_init(&res);
    bignum_from_int(&res, 1);

    bignum_init(&b);
    bignum_assign(&b, base);

    long long e = exp;

    while (e > 0) {
        if (e & 1) {
            bignum_mul(&res, &b, &tmp);
            bignum_mod(&tmp, mod, &res);
        }

        bignum_mul(&b, &b, &tmp);
        bignum_mod(&tmp, mod, &b);

        e >>= 1;
    }

    bignum_assign(result, &res);
}

/* Encode message as sequence of bignum blocks in base 256.
   Each block m satisfies m < modulus.
   Returns 0 on success, -1 on error. */
int message_to_blocks(const char* msg,
    struct bn* blocks,
    int max_blocks,
    int* out_blocks,
    struct bn* modulus) {
    struct bn base256;
    struct bn current;
    struct bn tmp;
    struct bn ch;
    struct bn candidate;
    struct bn zero;

    bignum_init(&base256);
    bignum_from_int(&base256, 256);

    bignum_init(&current);
    bignum_init(&tmp);
    bignum_init(&ch);
    bignum_init(&candidate);
    bignum_init(&zero);

    *out_blocks = 0;

    for (int i = 0; msg[i] != '\0'; ++i) {
        unsigned char c = (unsigned char)msg[i];

        /* candidate = current * 256 + c */
        bignum_mul(&current, &base256, &tmp);
        bignum_from_int(&ch, c);
        bignum_add(&tmp, &ch, &candidate);

        /* if candidate >= modulus, finalize current block and start new */
        if (!bignum_is_zero(&current) &&
            bignum_cmp(&candidate, modulus) >= 0) {

            if (*out_blocks >= max_blocks) {
                return -1;
            }

            bignum_assign(&blocks[*out_blocks], &current);
            (*out_blocks)++;

            /* start new block with just this byte */
            bignum_from_int(&current, c);
        }
        else {
            /* safe to grow current block */
            bignum_assign(&current, &candidate);
        }
    }

    /* final block */
    if (!bignum_is_zero(&current)) {
        if (*out_blocks >= max_blocks) {
            return -1;
        }
        bignum_assign(&blocks[*out_blocks], &current);
        (*out_blocks)++;
    }

    return 0;
}

/* Decode a single bignum block (base-256) back to bytes.
   Returns number of bytes written on success, -1 on error. */
int block_to_bytes(struct bn* in, char* out, int maxlen) {
    struct bn base;
    struct bn q, r;
    struct bn t;
    struct bn zero;
    int idx = 0;

    bignum_init(&base);
    bignum_init(&q);
    bignum_init(&r);
    bignum_init(&t);
    bignum_init(&zero);

    bignum_from_int(&base, 256);
    bignum_assign(&t, in);

    /* Extract bytes in reverse order using division by 256 */
    while (!bignum_is_zero(&t)) {
        if (idx >= maxlen - 1) {
            return -1; // not enough space
        }

        /* q = t / 256, r = t % 256 */
        bignum_divmod(&t, &base, &q, &r);

        int c = bignum_to_int(&r);
        if (c < 0) c = 0;
        if (c > 255) c = 255;

        out[idx++] = (char)c;
        bignum_assign(&t, &q);
    }

    /* reverse bytes to restore original order */
    for (int i = 0; i < idx / 2; ++i) {
        char tmpc = out[i];
        out[i] = out[idx - 1 - i];
        out[idx - 1 - i] = tmpc;
    }

    out[idx] = '\0';
    return idx;
}
