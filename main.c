#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <openssl/bn.h>

// Platform-specific headers for the 'sleep' function
#ifdef _WIN32 
    #include <windows.h>
#else 
    #include <unistd.h>
#endif

#include "decryption.c"
#include "Encryption.c"
#include "generer_noegler.c"

#define BUFFER_SIZE 10000

void keyselection(void);
void sentenceinput(void);
void menuselection(void);

char sentence[BUFFER_SIZE];
char fname_a[256], fname_b[256];
int valg_a, valg_b;
int encryptordecrypt;

// OpenSSL BIGNUMs for public key
BIGNUM *n_key = NULL;
BIGNUM *e_key = NULL;

int main(void) {
    // Initialize OpenSSL BIGNUMs
    n_key = BN_new();
    e_key = BN_new();

    printf("\033[36m\nP1 Encryption Program (OpenSSL Edition)\n\n\033[0m");
    sleep(1);

    printf("Do you wish to encrypt a message or do you wish to decrypt.- 1 = \033[32mencryption\033[0m, 2 = \033[34mdecryption\033[0m\n");
    printf("Enter your choice: ");
    scanf("%d",  &encryptordecrypt);
    getchar(); // consume newline

    if (encryptordecrypt == 1) {
        // Encryption flow
        menuselection();  // gets valg_a, key, and sentence

        // --- ENCRYPTION SECTION ---
        int len = (int)strlen(sentence);
        printf("Encrypting %d bytes...\n", len);

        FILE* f3 = fopen("crypted.txt", "w"); 
        if (f3 == NULL) {
            printf("Filen kunne ikke aabnes\n");
            exit(EXIT_FAILURE);
        }

        // Calculate max bytes per block.
        // RSA modulus size in bytes - 11 (padding) is safe for PKCS1, 
        // but for raw RSA (textbook), we just need message < modulus.
        // To be safe and simple, let's use (BN_num_bytes(n) - 1).
        int key_bytes = BN_num_bytes(n_key);
        int block_size = key_bytes - 1; 
        if (block_size < 1) block_size = 1; // Fallback

        printf("Block size: %d bytes\n", block_size);
        printf("Encrypted blocks: ");

        BN_CTX *ctx = BN_CTX_new();
        BIGNUM *m = BN_new();
        BIGNUM *c = BN_new();

        for (int i = 0; i < len; i += block_size) {
            // Determine chunk size
            int chunk_len = block_size;
            if (i + chunk_len > len) {
                chunk_len = len - i;
            }

            // Convert chunk to BIGNUM
            // BN_bin2bn expects big-endian, so we just pass the pointer
            BN_bin2bn((unsigned char*)&sentence[i], chunk_len, m);

            // Encrypt
            rsa_encrypt_block(c, m, n_key, e_key, ctx);

            // Print/Write as Hex
            char *c_hex = BN_bn2hex(c);
            printf("%s ", c_hex);
            fprintf(f3, "%s ", c_hex); 
            OPENSSL_free(c_hex);
        }
        printf("\n");

        BN_free(m);
        BN_free(c);
        BN_CTX_free(ctx);
        fclose(f3);
        printf("Your encrypted message is located in the text file 'crypted.txt'\n");
    }
    else if (encryptordecrypt == 2) {
        // Decryption flow
        decryption();
    }
    else {
        printf("fejl input!\n");
    }

    printf("Retur til hovedmenu.\n");
    
    // Cleanup
    BN_free(n_key);
    BN_free(e_key);
    
    return 0;
}

void menuselection(void)
{
    printf("\n\033[32m--- Starting encryption flow ---\033[0m\n");

    // --- INPUT SECTION ---
    printf("Do you wish to encrypt from a directly in the terminal or from a file (1 for terminal and 0 for file) \n");
    printf("Enter your choice: ");
    scanf("%d", &valg_a);
    getchar(); // Consume newline

    keyselection();
}

void keyselection(void)
{
    char line[256];

    // --- KEY SECTION ---
    printf("Do you wish to use an existing key pair or generate a new one? (1 for existing, 0 for new)\n");
    printf("Enter your choice: ");
    if (!fgets(line, sizeof(line), stdin)) {
        printf("Error reading input.\n");
        exit(EXIT_FAILURE);
    }
    if (sscanf(line, "%d", &valg_b) != 1) {
        printf("Invalid input.\n");
        exit(EXIT_FAILURE);
    }

    if (valg_b == 1) {
        // Use existing key pair
        printf("\nWhat is the name of the text file containing the key pair (n)?\n");
        printf("Enter the filename: ");
        if (!fgets(fname_b, sizeof(fname_b), stdin)) {
            printf("Error reading filename.\n");
            exit(EXIT_FAILURE);
        }
        fname_b[strcspn(fname_b, "\n")] = '\0';

        FILE* f2 = fopen(fname_b, "r");
        if (f2 == NULL) {
            printf("!FAILED TO OPEN THE FILE!\n");
            exit(EXIT_FAILURE);
        }

        char fileline[2048];
        int found = 0;

        while (fgets(fileline, sizeof(fileline), f2) != NULL) {
            if (strstr(fileline, "public key") != NULL) {
                // Format: public key key: (n=HEX, e=HEX)
                char* n_ptr = strstr(fileline, "n=");
                char* e_ptr = strstr(fileline, "e=");
                
                if (n_ptr && e_ptr) {
                    n_ptr += 2;
                    char *comma = strchr(n_ptr, ',');
                    if (comma) *comma = '\0';
                    BN_hex2bn(&n_key, n_ptr);

                    e_ptr += 2;
                    char *paren = strchr(e_ptr, ')');
                    if (paren) *paren = '\0';
                    BN_hex2bn(&e_key, e_ptr);

                    found = 1;
                    break;
                }
            }
        }
        fclose(f2);

        if (!found) {
            printf("!Could not find public key in file!\n");
            exit(EXIT_FAILURE);
        }
        
        char *n_str = BN_bn2hex(n_key);
        printf("Here is your keypair n: '%s'\n", n_str);
        OPENSSL_free(n_str);
        
        sentenceinput();
    }
    else if (valg_b == 0) {
        // Generate new key pair
        generate_keys();

        // Reload the generated key
        // For simplicity, we just ask the user to point to keys.txt again or we could auto-load it.
        // Let's auto-load from keys.txt since we just wrote it.
        
        printf("\nKeys generated. Loading from keys.txt...\n");
        FILE* f2 = fopen("keys.txt", "r");
        if (f2 == NULL) {
            printf("!FAILED TO OPEN THE FILE!\n");
            exit(EXIT_FAILURE);
        }
        
        // We need to find the LAST key in the file if multiple exist.
        // But generate_keys appends, so we can just read through.
        char fileline[2048];
        int found = 0;
        
        // Simple approach: read line by line, if we find a public key, parse it. 
        // The last one parsed will be the one we use.
        while (fgets(fileline, sizeof(fileline), f2) != NULL) {
            if (strstr(fileline, "public key") != NULL) {
                char* n_ptr = strstr(fileline, "n=");
                char* e_ptr = strstr(fileline, "e=");
                
                if (n_ptr && e_ptr) {
                    n_ptr += 2;
                    char *comma = strchr(n_ptr, ',');
                    if (comma) *comma = '\0';
                    BN_hex2bn(&n_key, n_ptr);

                    e_ptr += 2;
                    char *paren = strchr(e_ptr, ')');
                    if (paren) *paren = '\0';
                    BN_hex2bn(&e_key, e_ptr);

                    found = 1;
                }
            }
        }
        fclose(f2);

        if (!found) {
            printf("!Could not find public key in file!\n");
            exit(EXIT_FAILURE);
        }

        char *n_str = BN_bn2hex(n_key);
        printf("Here is your keypair n: '%s'\n", n_str);
        OPENSSL_free(n_str);

        sentenceinput();
    }
    else {
        printf("!You have given impossible instructions!\n");
        exit(EXIT_FAILURE);
    }
}

void sentenceinput(void)
{
    if (valg_a == 1) {
        printf("Type the sentence that you wish to have encrypted:\n");
        printf("Enter the sentence: ");
        fgets(sentence, BUFFER_SIZE, stdin);
        sentence[strcspn(sentence, "\n")] = '\0';
    }
    else if (valg_a == 0) {
        printf("What is the name of the file that to encrypt from?\n");
        printf("Enter the filename: ");
        scanf("%s", fname_a);
        getchar(); // Consume newline

        FILE* f1 = fopen(fname_a, "r");
        if (f1 == NULL) {
            printf("!FAILED TO OPEN THE FILE!\n");
            exit(EXIT_FAILURE);
        }
        // Read up to BUFFER_SIZE - 1
        size_t read_len = fread(sentence, 1, BUFFER_SIZE - 1, f1);
        sentence[read_len] = '\0';
        fclose(f1);
        
        // Remove trailing newline if present (optional, but keeps consistency)
        size_t len = strlen(sentence);
        if (len > 0 && sentence[len-1] == '\n') {
            sentence[len-1] = '\0';
        }
    }
    else {
        printf("!You have given impossible Instructions!\n");
        exit(EXIT_FAILURE);
    }

    printf("\n\nHere is your sentence:\n %s\n", sentence);
}

