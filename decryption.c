#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/bn.h>

// Platform-specific headers for the 'sleep' function
#ifdef _WIN32 
    #include <windows.h>
#else 
    #include <unistd.h>
#endif

#define MAX_TEXT_LENGTH 10000 // Increased buffer size

// Prototypes
void laes_privat_noegle(BIGNUM *n, BIGNUM *d);
int laes_krypteret_input(BIGNUM **ciphertext, int max_blocks);
void dekrypter_data(BIGNUM **ciphertext, int num_blocks, BIGNUM *n, BIGNUM *d, char *decrypted_message);
void udskriv_besked(char *message);
void save_message(char *message);

int decryption(void) {
    BIGNUM *n = BN_new();
    BIGNUM *d = BN_new();
    BIGNUM *ciphertext[MAX_TEXT_LENGTH]; // Array of BIGNUM pointers
    char decrypted_message[MAX_TEXT_LENGTH];
    int num_blocks = 0;

    printf("\n\033[36m--- Decryption flow started ---\n\n\033[0m");
    sleep(1);

    // 1. Read Private Key
    laes_privat_noegle(n, d);
    sleep(1);

    if (BN_is_zero(n) || BN_is_zero(d)) {
        printf("Error: Could not load keys.\n");
        return 1;
    }
    
    char *n_str = BN_bn2hex(n);
    char *d_str = BN_bn2hex(d);
    printf("Keys loaded: n=\033[32m%s\033[0m, d=\033[34m%s\033[0m\n\n", n_str, d_str);
    OPENSSL_free(n_str);
    OPENSSL_free(d_str);

    // 2. Read Encrypted Input
    // Initialize BIGNUM pointers for ciphertext array
    for(int i=0; i<MAX_TEXT_LENGTH; i++) ciphertext[i] = BN_new();
    
    num_blocks = laes_krypteret_input(ciphertext, MAX_TEXT_LENGTH);

    // 3. Decrypt
    dekrypter_data(ciphertext, num_blocks, n, d, decrypted_message);

    // 4. Show Result
    udskriv_besked(decrypted_message);

    sleep(3);
    save_message(decrypted_message);

    // Cleanup
    BN_free(n);
    BN_free(d);
    for(int i=0; i<MAX_TEXT_LENGTH; i++) BN_free(ciphertext[i]);

    return 0;
}

void laes_privat_noegle(BIGNUM *n, BIGNUM *d) {
    FILE *f2 = fopen("keys.txt", "r");
    if (f2 == NULL) {
        printf("ERROR 404 CANNOT OPEN FILE\n");
        return;
    }

    char line[2048]; // Increased buffer for long keys
    while (fgets(line, sizeof(line), f2) != NULL) {
        if (strstr(line, "private key") != NULL) {
            char *n_ptr = strstr(line, "n=");
            char *d_ptr = strstr(line, "d=");
            
            if (n_ptr && d_ptr) {
                // Extract hex strings manually because sscanf with %s might be tricky with delimiters
                // Format: private key: (n=HEX, d=HEX)
                
                // Advance past "n="
                n_ptr += 2;
                char *comma = strchr(n_ptr, ',');
                if (comma) *comma = '\0'; // Terminate n string temporarily
                BN_hex2bn(&n, n_ptr);
                
                // Advance past "d="
                d_ptr += 2;
                char *paren = strchr(d_ptr, ')');
                if (paren) *paren = '\0'; // Terminate d string
                BN_hex2bn(&d, d_ptr);
                
                break;
            }
        }
    }
    fclose(f2);
}

int laes_krypteret_input(BIGNUM **ciphertext, int max_blocks) {
    char fname_a[256];
    printf("What is your encrypted text file called?\n");
    printf("Enter the filename: ");
    scanf("%s", fname_a);
    
    FILE *f1 = fopen(fname_a, "r");
    if(f1 == NULL){
        printf("!FAILED TO OPEN THE FILE!\n");
        exit(EXIT_FAILURE);
    }

    int i = 0;
    char hex_buf[2048];
    // Read space-separated hex strings
    while (i < max_blocks && fscanf(f1, "%s", hex_buf) == 1) {
        BN_hex2bn(&ciphertext[i], hex_buf);
        i++;
    }
    fclose(f1);
    return i;
}

void dekrypter_data(BIGNUM **ciphertext, int num_blocks, BIGNUM *n, BIGNUM *d, char *decrypted_message) {
    BN_CTX *ctx = BN_CTX_new();
    BIGNUM *m = BN_new();
    int char_index = 0;

    // Determine block size based on key size (approximate)
    // For simplicity, we assume the same packing logic as encryption:
    // We need to know how many bytes were packed. 
    // In this upgraded version, let's assume we pack as many bytes as possible < modulus.
    // However, to keep it compatible with the simple 'main.c' logic we are about to write,
    // let's assume a fixed block size or unpack until 0.
    
    // NOTE: The user asked for "max size for a message is longer than 250".
    // We will implement a dynamic unpacking.
    
    for(int i = 0; i < num_blocks; i++){
        // m = c^d mod n
        BN_mod_exp(m, ciphertext[i], d, n, ctx);
        
        // Convert BIGNUM m back to bytes
        // BN_bn2bin returns big-endian bytes
        int num_bytes = BN_num_bytes(m);
        unsigned char *buf = malloc(num_bytes);
        BN_bn2bin(m, buf);
        
        // Append to message
        for(int j=0; j<num_bytes; j++) {
            decrypted_message[char_index++] = (char)buf[j];
        }
        free(buf);
    }
    decrypted_message[char_index] = '\0';
    
    BN_free(m);
    BN_CTX_free(ctx);
}

void udskriv_besked(char *message) {
    printf("\nDecrypted message:\n%s\n", message);
}

void save_message(char *message){
    int i = 0;
    while (i == 0){
        char fname_a[256];
        int choice;
        printf("\n\n\nDo you want to save the message as a .txt file?\n");
        printf("0. No\n");
        printf("1. Yes save as a .txt\n");
        sleep(1);
        printf("\nEnter your choice: ");

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n'); 
            choice = 0; 
        }

        switch (choice) {
        case 1:
            printf("\nWhat would you like the file to be called?\n");
            scanf("%s", fname_a);
            if (strstr(fname_a, ".txt") == NULL) {
                strcat(fname_a, ".txt");
            }
            FILE *f3 = fopen(fname_a, "w");
            if(f3 == NULL){
                printf("!FAILED TO OPEN THE FILE!\n");
                exit(EXIT_FAILURE);
            }
            fprintf(f3, "%s", message);
            fclose(f3);
            i = 1;
            printf("\nFile saved as %s\n", fname_a);
            break;
        case 0:
            i = 1;
            break;
        default:
            printf("Invalid choice. Please try again.\n");
            break;
        }
    }
}


