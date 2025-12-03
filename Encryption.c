#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/bn.h>

// Encrypt a message (as a BIGNUM) using public key (n, e)
// Result is stored in 'result'
void rsa_encrypt_block(BIGNUM *result, BIGNUM *msg, BIGNUM *n, BIGNUM *e, BN_CTX *ctx) {
    if (!BN_mod_exp(result, msg, e, n, ctx)) {
        fprintf(stderr, "Error: BN_mod_exp failed\n");
        exit(EXIT_FAILURE);
    }
}