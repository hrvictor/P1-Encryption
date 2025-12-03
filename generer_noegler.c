#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <openssl/bn.h>

// Function to handle OpenSSL errors
void print_openssl_error(const char *msg) {
    fprintf(stderr, "OpenSSL Error: %s\n", msg);
    // ERR_print_errors_fp(stderr); // Uncomment if you want detailed OpenSSL errors
}

int generate_keys(void) {
    printf("Generating RSA keys with OpenSSL... This may take a moment.\n");

    // Initialize BIGNUM structures
    BIGNUM *p = BN_new();
    BIGNUM *q = BN_new();
    BIGNUM *n = BN_new();
    BIGNUM *e = BN_new();
    BIGNUM *d = BN_new();
    BIGNUM *phi = BN_new();
    BIGNUM *p_minus_1 = BN_new();
    BIGNUM *q_minus_1 = BN_new();
    BIGNUM *one = BN_new();
    BN_CTX *ctx = BN_CTX_new();

    if (!p || !q || !n || !e || !d || !phi || !p_minus_1 || !q_minus_1 || !one || !ctx) {
        print_openssl_error("Failed to allocate BIGNUMs");
        return 1;
    }

    // Set constant 1
    BN_one(one);

    // Set e = 65537
    BN_set_word(e, 65537);

    // Generate prime p (1024 bits)
    if (!BN_generate_prime_ex(p, 1024, 0, NULL, NULL, NULL)) {
        print_openssl_error("Failed to generate prime p");
        return 1;
    }
    printf("Generated prime p.\n");

    // Generate prime q (1024 bits)
    if (!BN_generate_prime_ex(q, 1024, 0, NULL, NULL, NULL)) {
        print_openssl_error("Failed to generate prime q");
        return 1;
    }
    printf("Generated prime q.\n");

    // Calculate n = p * q
    if (!BN_mul(n, p, q, ctx)) {
        print_openssl_error("Failed to calculate n");
        return 1;
    }

    // Calculate phi = (p-1) * (q-1)
    BN_sub(p_minus_1, p, one);
    BN_sub(q_minus_1, q, one);
    if (!BN_mul(phi, p_minus_1, q_minus_1, ctx)) {
        print_openssl_error("Failed to calculate phi");
        return 1;
    }

    // Calculate d = e^-1 mod phi
    if (!BN_mod_inverse(d, e, phi, ctx)) {
        print_openssl_error("Failed to calculate private key d. Ensure gcd(e, phi) = 1.");
        return 1;
    }

    // Convert to hex strings for storage
    char *n_hex = BN_bn2hex(n);
    char *e_hex = BN_bn2hex(e);
    char *d_hex = BN_bn2hex(d);

    printf("\nKeys generated successfully!\n");
    printf("Public Key (n, e) saved to keys.txt\n");

    // Save to file
    FILE *fptr = fopen("keys.txt", "a");
    if (fptr == NULL) {
        printf("Error opening keys.txt for writing.\n");
        return 1;
    }
    
    // Format: public key key: (n=HEX, e=HEX)
    //         private key: (n=HEX, d=HEX)
    fprintf(fptr, "public key key: (n=%s, e=%s)\nprivate key: (n=%s, d=%s)\n", n_hex, e_hex, n_hex, d_hex);
    fclose(fptr);

    // Cleanup
    OPENSSL_free(n_hex);
    OPENSSL_free(e_hex);
    OPENSSL_free(d_hex);
    BN_free(p);
    BN_free(q);
    BN_free(n);
    BN_free(e);
    BN_free(d);
    BN_free(phi);
    BN_free(p_minus_1);
    BN_free(q_minus_1);
    BN_free(one);
    BN_CTX_free(ctx);

    return 0;
}