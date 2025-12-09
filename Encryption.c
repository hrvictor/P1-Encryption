#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#define buffer 250
#define BLOCK_SIZE 2

// Helper function for RSA math to prevent overflow
long long modExp(long long base, long long exp, long long mod) {
    long long result = 1;
    base = base % mod;
    while (exp > 0) {
        if (exp % 2 == 1) result = (result * base) % mod;
        exp = exp >> 1;
        base = (base * base) % mod;
    }
    return result;
}

int ikmain(void) {

    printf("\n--- Starting Encryption flow ---\n");

    char sentence[buffer];
    char fname_a[buffer], fname_b[buffer];
    int choice_a, choice_b;

    // Initialized variables to be safe
    long long int key = 0, exp = 65537;
 
    if (choice_a == 1) {
        printf("Type the sentence that you wish to have encrypted:\n");
        fgets(sentence, buffer, stdin);
        sentence[strcspn(sentence, "\n")] = 0;
    }
    else if (choice_a == 0) {
        printf("What is the name of the file that to encrypt from?\n");
        scanf("%s", fname_a);
        FILE* f1 = fopen(fname_a, "r");
        if (f1 == NULL) {
            printf("!FAILED TO OPEN THE FILE!\n");
            exit(EXIT_FAILURE);
        }
        fgets(sentence, buffer, f1);
        fclose(f1);
        sentence[strcspn(sentence, "\n")] = 0; // Strip newline from file input too
    }
    else {
        printf("!You have given impossible Instructions!\n");
        exit(EXIT_FAILURE);
    }

    printf("\n\nHere is your sentence:\n %s\n", sentence);

    // --- KEY SECTION ---
    printf("Do you wish to use an existing key pair or generate or new one? (1 for existing and 0 for new)\n");
    scanf("%d", &choice_b);

    if (choice_b == 1) {
        printf("\nwhat is the name of the text file containing the key pair (n)?\n");
        scanf("%s", fname_b);

        FILE* f2 = fopen(fname_b, "r");
        if (f2 == NULL) {
            printf("!FAILED TO OPEN THE FILE!\n");
            exit(EXIT_FAILURE);
        }

        // Parse the file for "public key key: (n=%llx"
        // We look for the line starting with "public key"
        char line[256];
        int found = 0;
        while (fgets(line, sizeof(line), f2) != NULL) {
            if (strstr(line, "public key") != NULL) {
                // Found the line, now extract n. 
                // Format: public key key: (n=6f249d, e=10001)
                char* n_ptr = strstr(line, "n=");
                if (n_ptr != NULL) {
                    sscanf(n_ptr, "n=%llx", &key);
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

        printf("Here is your keypair: '%lld' (hex: %llx)\n", key, key);
    }
    else if (choice_b == 0) {
        // Placeholder for key generation
        printf("Key generation not implemented yet. Using dummy key 3233 (example)\n");
        key = 3233; // Temporary fallback for testing
    }
    else {
        printf("!You have given impossible Instructions!\n");
        exit(EXIT_FAILURE);
    }

    // --- ENCRYPTION SECTION ---
    // We must encrypt in blocks to match the decryption logic
    int len = strlen(sentence);

    // Pad with nulls if needed
    if (len % BLOCK_SIZE != 0) {
        sentence[len] = '\0';
        len++;
    }

    int numBlocks = len / BLOCK_SIZE;
    long long int ciphertext[256];

    printf("The original text: '%s'\n", sentence);
    printf("Encrypted blocks: ");

    FILE* f3 = fopen("crypted.txt", "w"); // Use "w" to overwrite/start fresh
    if (f3 == NULL) {
        printf("File could not be opened\n");
        exit(EXIT_FAILURE);
    }

    for (int b = 0; b < numBlocks; b++) {
        long long int m = 0;
        // Pack characters into one integer
        for (int j = 0; j < BLOCK_SIZE; j++) {
            unsigned char ch = (unsigned char)sentence[b * BLOCK_SIZE + j];
            m = m * 256 + ch;
        }

        long long int c = modExp(m, exp, key);
        ciphertext[b] = c;

        printf("%lld ", c);
        fprintf(f3, "%lld ", c); // Write space-separated blocks
    }
    printf("\n");

    fclose(f3);
    printf("Your encrypted message is located in the text file 'crypted.txt'\n");

    printf("Return to main menu.\n");
    return 0;
}